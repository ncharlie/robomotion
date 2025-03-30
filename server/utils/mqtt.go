package utils

import (
	"errors"
	"log/slog"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func NewMqttClient(brokerUrl, username, password, clientId string) mqtt.Client {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(brokerUrl)
	opts.SetClientID(clientId)
	opts.SetUsername(username)
	opts.SetPassword(password)
	opts.SetCleanSession(false)

	opts.SetAutoReconnect(true)
	opts.SetMaxReconnectInterval(1 * time.Second)

	opts.SetOnConnectHandler(func(client mqtt.Client) {
		slog.Info("Connected to MQTT broker")
	})

	opts.SetConnectionLostHandler(func(client mqtt.Client, err error) {
		slog.Error("Connection lost", "error", err)
	})

	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		slog.Error("Failed to connect to MQTT broker", "error", token.Error())
		panic(token.Error())
	}

	return client
}

func DisconnectMqtt(client mqtt.Client) error {
	client.Disconnect(1000)
	slog.Warn("MQTT client disconnected")
	return nil
}

func Subscribe(client mqtt.Client, topic string, handler mqtt.MessageHandler) {
	token := client.Subscribe(topic, 0, handler)
	token.Wait()

	if token.Error() != nil {
		slog.Error("Failed to subscribe to topic", "error", token.Error())
		panic(token.Error())
	} else {
		slog.Info("Subscribed to topic", "topic", topic)
	}
}

func Publish(client mqtt.Client, topic, message string) error {
	token := client.Publish(topic, 0, false, message)
	if sent := token.WaitTimeout(5 * time.Second); !sent {
		slog.Error("Failed to publish message: Timeout", "topic", topic, "message", message)
		return errors.New("publish timeout")
	}

	if token.Error() != nil {
		slog.Error("Failed to publish message: Error", "error", token.Error())
		return token.Error()
	}

	return nil
}
