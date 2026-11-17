# KEPServerEX REST Contract

## Purpose

This contract defines the external interface between the ESP32 telemetry firmware and the KEPServerEX IoT Gateway REST Server Agent.

## Transport

- Protocol: HTTP/1.1
- Method: POST
- Server role: KEPServerEX REST Server Agent on the OT LAN
- Authentication: None (No Auth)
- Content-Type: `application/json`

## Endpoint

The endpoint is configured in `config.h` and should look like:

`http://<KEPSERVER_HOST>:<PORT>/<REST_ENDPOINT>`

Example:

`http://192.168.1.20:8080/api/telemetry`

## Request body

The firmware sends a JSON array in the exact KEPServerEX tag format:

```json
[
  {"id":"Tag_Nhiet_Do","v":25.4},
  {"id":"Tag_Do_Am","v":52.1}
]
```

### Field semantics

- `id`: KEPServerEX tag name or item identifier
- `v`: numeric measurement value for the tag

## Success criteria

- HTTP status `200 OK` indicates a successful telemetry write.
- A `2xx` response is treated as successful server acceptance.
- Response bodies may be empty or contain server diagnostic metadata; the firmware only needs the status code for health decisions.

## Failure handling

The firmware must treat the following as communication failures:

- Connection timeout
- DNS or IP routing issues
- HTTP non-2xx status code
- TCP disconnect occurred before request completion
- KEPServerEX server is unavailable or restarted

On failure:

- Update the server state to `OFFLINE` or `RETRYING`
- Trigger LED warning behavior
- Emit a Vietnamese Serial log entry with the error condition and retry timeline
- Keep local sensor sampling active without blocking the main loop

## Example success response

```http
HTTP/1.1 200 OK
Content-Type: application/json

{"status":"ok"}
```

## Example failure response

```http
HTTP/1.1 503 Service Unavailable
Content-Type: application/json

{"status":"server_unavailable"}
```

## Implementation notes

- The request payload is generated as a static char buffer using `snprintf()`.
- The firmware must not use dynamic `String` objects in request preparation.
- The HTTP client must not block the `loop()` path; all retry logic is handled through timed state transitions.
