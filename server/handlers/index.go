package handlers

import (
	"robomotion/services"
)

type APIHandler struct {
	Service *services.Service
}

func NewSubscribeHandler(service *services.Service) *APIHandler {
	return &APIHandler{
		Service: service,
	}
}
