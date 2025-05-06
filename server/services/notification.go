package services

import (
	"robomotion/models/dto"
	"time"
)

func (s *Service) GetNotification(robotId string, lastCheck *time.Time) (*dto.Notification, error) {
	noti := s.notificationRepository.GetNotification(robotId, lastCheck)
	if noti == nil {
		return nil, nil
	}

	notification := &dto.Notification{
		RobotId:   noti.RobotId,
		Type:      noti.Type,
		TimeStamp: noti.CreatedAt,
	}

	return notification, nil
}
