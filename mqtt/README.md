# Robomotion MQTT

Robomotion uses Netpie as an MQTT broker for robot-server communication.

## Credentials

1. Client ID
2. Token
3. Secret

## Interface List
- [Location Broadcasting](#location%20broadcasting)
- [Movement](#movement)
- [Notification](#notification)

## Location Broadcasting

- topic: @msg/location/{robot_id}

```json
{
    "x": 101.52,
    "y": 203.67,
    "z": 0
}
```

## Movement

- topic: @msg/move/{robot_id}

```text
F | B | L | R
```

## Notification

- topic: @msg/noti/{robot_id}

```json
{
    "type": "obstacle" | "zone"
}
```