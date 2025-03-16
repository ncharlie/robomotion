package services

import (
	"robomotion/repositories"
)

type Service struct {
	LogRepository *repositories.LogRepository
}

func NewService(
	logRepository *repositories.LogRepository,
) *Service {
	return &Service{
		LogRepository: logRepository,
	}
}
