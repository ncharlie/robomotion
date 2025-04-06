package services

import (
	"robomotion/repositories"
)

type Service struct {
	robotRepository        *repositories.RobotRepository
	beaconRepository       *repositories.BeaconRepository
	logRepository          *repositories.LogRepository
	notificationRepository *repositories.NotificationRepository
}

func NewService(
	robotRepository *repositories.RobotRepository,
	beaconRepository *repositories.BeaconRepository,
	logRepository *repositories.LogRepository,
	notificationRepository *repositories.NotificationRepository,
) *Service {
	return &Service{
		robotRepository:        robotRepository,
		beaconRepository:       beaconRepository,
		logRepository:          logRepository,
		notificationRepository: notificationRepository,
	}
}
