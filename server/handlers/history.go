package handlers

import (
	"log/slog"
	"net/http"
	"robomotion/models/dto"
	"robomotion/utils"
	"strconv"

	"github.com/goccy/go-json"
)

func (h *APIHandler) GetHistory(w http.ResponseWriter, r *http.Request) {
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

	page := params.Get("p")
	amount := params.Get("a")
	if page == "" {
		page = "1"
	}
	if amount == "" {
		amount = "50"
	}

	pageInt, err := strconv.ParseInt(page, 10, 64)
	if err != nil {
		logger.Error("failed to parse page", "error", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}
	amountInt, err := strconv.ParseInt(amount, 10, 64)
	if err != nil {
		logger.Error("failed to parse amount", "error", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	data, count, err := h.Service.GetHistory(robotId, pageInt, amountInt)
	if err != nil {
		logger.Error("failed to get history", "error", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	if len(data) == 0 {
		logger.Info("no history found")
		w.WriteHeader(http.StatusNotFound)
		return
	}

	type response struct {
		Robot []dto.Robot `json:"robots"`
		Count int64       `json:"count"`
	}
	res := response{
		Robot: data,
		Count: count,
	}

	// b, err := json.Marshal(res)
	// if err != nil {
	// 	logger.Error("failed to marshal response", "error", err)
	// 	w.WriteHeader(http.StatusInternalServerError)
	// 	return
	// }
	w.Header().Set("Content-Type", "application/json")
	// w.Write(b)
	json.NewEncoder(w).Encode(res)
	// w.WriteHeader(http.StatusOK)
	return
}
