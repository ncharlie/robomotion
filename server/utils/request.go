package utils

import "net/http"

func GetIP(r *http.Request) string {
	if r.Header.Get("x-forwarded-for") != "" {
		return r.Header.Get("x-forwarded-for")
	}
	return r.RemoteAddr
}

func GetAgent(r *http.Request) string {
	return r.Header.Get("user-agent")
}
