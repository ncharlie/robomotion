package services

import (
	"robomotion/models/dto"
)

func (s *Service) GetLocation(robotId string) (*dto.Robot, error) {
	robot := s.logRepository.GetLatestLocation(robotId)
	if robot == nil {
		return nil, nil
	}

	location := &dto.Robot{
		RobotId: robot.RobotId,
		X:       robot.Location.X,
		Y:       robot.Location.Y,
		Z:       robot.Location.Z,
		Heading: robot.Location.Heading,
		Speed:   robot.Location.Speed,
	}

	return location, nil
}
