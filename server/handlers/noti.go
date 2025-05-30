package handlers

import (
	"encoding/json"
	"fmt"
	"log/slog"
	"net/http"
	"robomotion/utils"
	"time"
)

func (h *APIHandler) GetNoti(w http.ResponseWriter, r *http.Request) {
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

	params := r.URL.Query()
	robotId := params.Get("id")
	if robotId == "" {
		logger.Error("robotId is required")
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	lastCheck := time.Now()

	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Access-Control-Expose-Headers", "Content-Type")

	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")

	// w.WriteHeader(http.StatusOK)

	clientGone := r.Context().Done()

	t := time.NewTicker(2 * time.Second)
	defer t.Stop()
	rc := http.NewResponseController(w)

	for {
		select {
		case <-clientGone:
			fmt.Println("client disconnected")
			t.Stop()
			return
		case <-t.C:
			data, err := h.Service.GetNotification(robotId, &lastCheck)
			if err != nil {
				logger.Error("failed to get location", "error", err)
				continue
			}
			if data != nil {
				// data to json
				jsonData, err := json.Marshal(data)
				if err != nil {
					logger.Error("failed to marshal data", "error", err)
					continue
				}
				if _, err = fmt.Fprintf(w, "event: Notification\ndata: %s\n\n", (string)(jsonData)); err != nil {
					return
				}
				if err = rc.Flush(); err != nil {
					return
				}
				lastCheck = data.TimeStamp
			}
		}
	}
}
