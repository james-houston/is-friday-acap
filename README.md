# is-friday-acap
This repository contains a Go API to check if current day of the week is a Friday, and an ACAP to read from the API and send an axevent with the status.

- `acap` directory contains the ACAP code written in C
- `api` contains the Go API that the ACAP calls

## Prompt
Make an ACAP that polls from the internet.

If the response from said API is:
```json
{
    "friday": true
}
```
Then let the ACAP trigger an event that says it's Friday.
- needs to be written in C
- nees to parse JSON (no `if stringContains("true")`)

## Implementation
the API schema is as follows:
```json
{
    "friday": boolean
}
```
So there are no other days of the week, and we only have to check the boolean state of `friday`. The ACAP polls the API every 5 seconds to check the value.

**! In this repo, the API currently randomly sends true/false to be able to demonstrate it's functionality without having to wait for Friday. I have included the block of code commented out in the API main src file to return true/false based on the actual day of the week**


In the ACAP I used a stateful axevent that has a persistent true/false state. Once this event is initialized, I polled the API every 5 seconds to check the value of `friday`. When the API returns `true` the state of the event is updated IFF the event state is currently set to `false`. The same happens in reverse when the API returns `false` and the event state is `true`