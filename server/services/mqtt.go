package services

import (
	"encoding/json"
	"log/slog"
	"robomotion/env"
	"robomotion/models/dto"
	"robomotion/models/entities"
	"robomotion/repositories"
	"robomotion/utils"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/go-playground/validator/v10"
)

type MqttService struct {
	logRepository  *repositories.LogRepository
	notiRepository *repositories.NotificationRepository
	client         mqtt.Client
	data           chan dto.Robot
	notification   chan dto.Notification
}

func NewMqttService(logRepository *repositories.LogRepository, notiRepository *repositories.NotificationRepository, client mqtt.Client) *MqttService {
	s := MqttService{
		logRepository:  logRepository,
		notiRepository: notiRepository,
		client:         client,
		data:           make(chan dto.Robot, 200),
		notification:   make(chan dto.Notification, 50),
	}

	utils.Subscribe(client, env.Get("mqtt.topic.location"), s.getLocationHandler())
	utils.Subscribe(client, env.Get("mqtt.topic.notification"), s.getNotificationHandler())

	go func() {
		for {
			select {
			case data := <-s.data:
				log := entities.Update{
					RobotId: data.RobotId,
					Location: entities.Location{
						X:       data.X,
						Y:       data.Y,
						Z:       data.Z,
						Heading: data.Heading,
						Speed:   data.Speed,
					},
				}

				if err := logRepository.InsertLog(&log); err != nil {
					slog.Error("Failed to insert log", "error", err)
				} else {
					slog.Info("Decoded c location", "data", data)
				}
			case data := <-s.notification:
				noti := entities.Notification{
					RobotId: data.RobotId,
					Type:    data.Type,
				}

				if err := notiRepository.Insert(&noti); err != nil {
					slog.Error("Failed to insert noti", "error", err)
				} else {
					slog.Info("Decoded c noti", "noti", data)
				}
			}
		}
	}()
	return &s
}

func (s *MqttService) getLocationHandler() mqtt.MessageHandler {
	return func(client mqtt.Client, msg mqtt.Message) {
		var data dto.Robot
		if err := json.Unmarshal(msg.Payload(), &data); err != nil {
			slog.Error("Failed to decode message", "error", err)
			return
		}

		validate := validator.New()
		if err := validate.Struct(data); err != nil {
			slog.Error("Validation failed", "error", err)
			return
		}
		s.data <- data
	}
}

func (s *MqttService) getNotificationHandler() mqtt.MessageHandler {
	return func(client mqtt.Client, msg mqtt.Message) {
		slog.Info("Received message", "topic", msg.Topic(), "payload", string(msg.Payload()))

		var data dto.Notification
		if err := json.Unmarshal(msg.Payload(), &data); err != nil {
			slog.Error("Failed to decode message", "error", err)
			return
		}

		validate := validator.New()
		if err := validate.Struct(data); err != nil {
			slog.Error("Validation failed", "error", err)
			return
		}
		s.notification <- data
	}
}

func (s *MqttService) MoveRobot(robotId string, dir dto.Direction) error {
	if err := utils.Publish(s.client, env.Get("mqtt.topic.movement")+"/"+robotId, string(dir)); err != nil {
		slog.Error("Publish error", "error", err)
		return err
	}

	return nil
}
