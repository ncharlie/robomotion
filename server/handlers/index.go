package handlers

import (
	"robomotion/services"
)

type APIHandler struct {
	Service     *services.Service
	MqttService *services.MqttService
}

func NewSubscribeHandler(service *services.Service, mqttService *services.MqttService) *APIHandler {
	return &APIHandler{
		Service:     service,
		MqttService: mqttService,
	}
}
