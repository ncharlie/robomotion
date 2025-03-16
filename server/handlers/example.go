package handlers

import (
	"fmt"
	"log/slog"
	"net/http"
	"robomotion/utils"

	"github.com/gorilla/mux"
)

func (h *APIHandler) GetPlans(w http.ResponseWriter, r *http.Request) {
	logger := slog.With("from", utils.GetIP(r), "url", r.URL)
	logger.Info("Incoming", "proto", r.Proto, "host", r.Host, "agent", utils.GetAgent(r))
	vars := mux.Vars(r)

	fmt.Fprintf(w, "Category: %v\n", vars["category"])
	w.WriteHeader(http.StatusOK)
	for k, v := range r.Header {
		fmt.Fprintf(w, "<%s>\t\t\t= %s\n", k, v[0])
	}
}
