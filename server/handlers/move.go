package handlers

import (
	"encoding/json"
	"log/slog"
	"net/http"
	"robomotion/models/dto"
	"robomotion/utils"
)

func (h *APIHandler) Move(w http.ResponseWriter, r *http.Request) {
	logger := slog.With("from", utils.GetIP(r), "url", r.URL)
	logger.Info("Incoming", "proto", r.Proto, "host", r.Host, "agent", utils.GetAgent(r))

	cookie, err := r.Cookie("session")
	if err != nil {
		logger.Error("failed to get cookie", "error", err)
		w.WriteHeader(http.StatusUnauthorized)
		return
	}
	if cookie.Value != "secretcokkieforthissession" {
		logger.Error("invalid cookie", "cookie", cookie.Value)
		w.WriteHeader(http.StatusUnauthorized)
		return
	}

	type request struct {
		RobotId   string `json:"robotId"`
		Direction string `json:"direction"`
	}
	var req request
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		logger.Error("failed to decode request", "error", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	if req.RobotId == "" {
		logger.Error("robotId is required")
		w.WriteHeader(http.StatusBadRequest)
		return
	}
	if req.Direction == "" {
		logger.Error("direction is required")
		w.WriteHeader(http.StatusBadRequest)
		return
	}
	var dir dto.Direction

	switch req.Direction {
	case "F", "f":
		dir = dto.Forward
	case "B", "b":
		dir = dto.Backward
	case "L", "l":
		dir = dto.Left
	case "R", "r":
		dir = dto.Right
	case "S", "s":
		dir = dto.Stop
	default:
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	if err := h.MqttService.MoveRobot(req.RobotId, dir); err != nil {
		logger.Error("failed to move robot", "error", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Cache-Control", "no-cache")
	w.WriteHeader(http.StatusOK)
}
