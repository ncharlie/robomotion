package services

import (
	"encoding/json"
	"log/slog"
	"robomotion/env"
	"robomotion/models/dto"
	"robomotion/repositories"
	"robomotion/utils"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/go-playground/validator/v10"
)

type MqttService struct {
	LogRepository *repositories.LogRepository
	client        mqtt.Client
	data          chan dto.Robot
	notification  chan dto.Robot
}

func NewMqttService(logRepository *repositories.LogRepository, client mqtt.Client) *MqttService {
	s := MqttService{
		LogRepository: logRepository,
		client:        client,
		data:          make(chan dto.Robot, 200),
		notification:  make(chan dto.Robot, 50),
	}

	utils.Subscribe(client, env.Get("mqtt.topic.location"), s.getLocationHandler())
	utils.Subscribe(client, env.Get("mqtt.topic.notification"), s.getNotificationHandler())

	go func() {
		for {
			select {
			case data := <-s.data:
				slog.Info("Decoded c location", "data", data)
			case data := <-s.notification:
				slog.Info("Decoded c noti", "data", data)
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
		s.notification <- data
	}
}
