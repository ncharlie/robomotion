package services

import (
	"robomotion/models/dto"
)

func (s *Service) GetHistory(robotId string, page, pageSize int64) ([]dto.Robot, int64, error) {
	logs, count, err := s.logRepository.GetLogsPaginated(robotId, page, pageSize)
	if err != nil {
		return nil, 0, err
	}

	if len(logs) == 0 {
		return nil, 0, nil
	}

	robots := make([]dto.Robot, 0, len(logs))
	for _, robot := range logs {
		robots = append(robots, dto.Robot{
			RobotId:   robot.RobotId,
			X:         robot.Location.X,
			Y:         robot.Location.Y,
			Z:         robot.Location.Z,
			Heading:   robot.Location.Heading,
			Speed:     robot.Location.Speed,
			TimeStamp: &robot.CreatedAt,
		})
	}

	return robots, count, nil
}
