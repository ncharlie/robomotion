package handlers

import (
	"encoding/json"
	"log/slog"
	"net/http"
	"robomotion/utils"
)

func (h *APIHandler) Auth(w http.ResponseWriter, r *http.Request) {
	logger := slog.With("from", utils.GetIP(r), "url", r.URL)
	logger.Info("Incoming", "proto", r.Proto, "host", r.Host, "agent", utils.GetAgent(r))
	// vars := mux.Vars(r)

	type request struct {
		Username string `json:"username"`
		Password string `json:"password"`
	}
	var req request
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		logger.Error("failed to decode request", "error", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	if req.Username != "admin" || req.Password != "presetpassword" {
		w.WriteHeader(http.StatusUnauthorized)
		// fmt.Fprint(w, "unauthorized")
		return
	}

	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Cache-Control", "no-cache")
	http.SetCookie(w, &http.Cookie{
		Name:     "session",
		Value:    "secretcokkieforthissession",
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
		MaxAge:   10800, // 3 hours
	})
	http.SetCookie(w, &http.Cookie{
		Name:     "sessionCheck",
		Value:    "true",
		HttpOnly: false,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
		MaxAge:   10000, // almost 3 hours
	})
	w.WriteHeader(http.StatusOK)
}
