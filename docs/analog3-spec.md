# Analog3 Communication Protocol Specification

## Introduction

TBD

## CAN Identifier Assignments

| ID (hex)          | ID (binary)                      | Name              | Owner             | Notes                                                    |
| ----------------- | -------------------------------- | ----------------- | ----------------- | -------------------------------------------------------- |
| 0x100             | 001 0000 0000                    | MIDI timing clock | MIDI Receiver     | Forwarded from MIDI                                      |
| 0x101 -<br> 0x13F | 001 0000 0001 -<br>001 0011 1111 | MIDI voice        | MIDI Receiver     | Conveys voice messages                                   |
| 0x140             | 001 0100 0000                    | MIDI real time    | MIDI Receiver     | Forwarded MIDI real time messages<br>except timing clock |
| 0x141             | 001 0100 0001                    | MIDI channel      | MIDI Receiver     | Forwarded MIDi Channel messages                          |
|                   |                                  |                   |                   |                                                          |
| 0x700             | 111 0000 0000                    | Control message   | Mission Control   | Messages from the mission control                        |
| 0x701 -<br>0x7FF  | 111 0000 0000 -<br>111 1111 1111 | Module ID         | Individual Module | Dynamically assigned module ID                           |

## Message Details

Messages are defined in this section. Several messages expect some responses from one or
more remote modules. A set of request and expected responses is called a *stream*.
Since streams are not tracked by ID, a module can make only one stream for a remote
module at a time (it is fine to create concurrent streams of the same type
for different modules).

Note: This design is to make lengths of messages as short as possible. We keep this policy
until concurrent operations are necessary.

### MIDI Timing Clock Message
#### ID
0x100 (001 0000 0000)

#### Owner
The MIDI receiver

#### Payload
None

#### Description
Messages with this ID are forwarded from MIDI Timing Clock. The message should contain no payload data.

### MIDI Voice Message
#### ID
0x101 (001 0000 0001) to 0x13f (001 0011 1111) - 63 voices at maximum

#### Owner
MIDI Receiver

#### Payload
A MIDI Voice message can contain multiple of the following sub-messages.
All messages are for broadcasting. No responses are expected.

| Opcode | Op Name           | Length | Operands         |
| ------ | ----------------- | -----: | ---------------- |
| 0x07   | SET_NOTE          |      2 | note_num (0-127) |
| 0x08   | GATE_OFF          |      1 |                  |
| 0x09   | GATE_ON           |      2 | velocity (0-127) |
| 0x0A   | POLY_KEY_PRESSURE |      2 | pressure (0-127) |

#### Description
Used for controlling corresponding voice modules.

### MIDI Real Time Message
#### ID
0x140 (001 0100 0000)

#### Owner
MIDI Receiver

#### Payload
1 byte real time message status

#### Description
MIDI real time messages are forwarded here except timing clocks.

### MIDI Channel Message
#### ID
0x141 (001 0100 0001)

#### Owner
MIDI Receiver

#### Payload
All messages are for broadcasting. No responses are expected.

| Opcode | Op Name          | Length | Operands                              |
| ------ | ---------------- | -----: | ------------------------------------- |
| 0x0B   | CONTROL_CHANGE   |      3 | control_number (0-127), value (0-127) |
| 0x0C   | PROGRAM_CHANGE   |      2 | program_number (0-127)                |
| 0x0D   | CHANNEL_PRESSURE |      2 | value (0-127)                         |
| 0x0E   | PITCH_BEND       |      3 | value_h (0-64), value_l (0-127)       |

Control numbers follow the MIDI convention.
See [https://midi.org/midi-1-0-control-change-messages]
or [https://anotherproducer.com/online-tools-for-musicians/midi-cc-list/].

### Mission Control Message
#### ID
0x700 (111 0000 0000)

In the Analog3 protocol, CAN standard IDs from 0x700 to 0x7FF are used for
identifiers of modules. These IDs are exchanged as numbers
in range 0x00-0xFF. The CAN ID should be `0x700 + module_id`.

#### Owner
Mission Control

#### Payload

| Opcode | Op Name          | Length | Operands                                          | Expected<br>Response          |
| ------ | ---------------- | ------ | ------------------------------------------------- | ----------------------------- |
| 0x01   | SIGN_IN          | 1      |                                                   | IM_SIGN_IN or<br>IM_NOTIFY_ID |
| 0x02   | ASSIGN_MODULE_ID | 5      | module_uid[4] (0-536870911), module_id (1-255)    |                               |
| 0x03   | PING             | 2 or 3 | module_id (1-255), o/enable_visual_response (0/1) | PING_REPLY                    |
| 0x04   | REQUEST_NAME     | 2      | module_id (1-255)                                 | NAME_REPLY                    |
| 0x05   | CONTINUE_NAME    | 2      | module_id (1-255)                                 | NAME_REPLY                    |


#### Description
TBD

### Individual Module Message
#### ID
from 0x701 (111 0000 0001) to 0x7FF (111 1111 1111)

Each module must have a unique ID in this range. The Mission Control assigns them dynamically. See section Module ID Assignment (TBD).

In the Analog3 protocol, CAN standard IDs from 0x700 to 0x7FF are used for
identifiers of modules. These IDs are exchanged as numbers
in range 0x00-0xFF. The CAN ID should be `0x700 + module_id`.

#### Owner
Individual module

#### Payload

| Opcode | Op Name    | Length | Operands     |
| ------ | ---------- | ------ | ------------ |
| 0x01   | PING_REPLY | 1      |              |
| 0x02   | NAME_REPLY | >2     | field "name" |

### Module administration message
#### ID
A randomly generated 29-bit ID. This is called UID (unique ID).
The UID must be persisted in the module.

#### Owner
Individual module

#### Payload

| Opcode | Op Name           | Length | Operands          | Expected<br>Response |
| ------ | ----------------- | ------ | ----------------- | -------------------- |
| 0x01   | IM_SIGN_IN        | 1      |                   | ASSIGN_MODULE_ID     |
| 0x02   | IM_NOTIFY_ID      | 2      | module_id (1-255) |                      |
| 0x03   | IM_REQ_UID_CANCEL | 1      |                   |                      |

## Special Operations

### Sign-In Procedure for Individual Module
When a module joins the network for the first time, it should start the sign-in
procedure as follows:

- The joining module sends SIGN_IN message with its UID.
- If another individual module finds the UID conflicting with its own UID, the module must send
  a REQ_UID_CANCEL message with this UID.
    - Both the joining module and existing module must reassign the UID
      and must start over sign-in procedures upon receiving this message.
    - The Mission Control should invalidate this UID.
- The joining module should wait for the reply from the Mission Control, but also
should start its own function. Each module should be designed to function without
presence of the Mission Control. The Mission Control may or may not exist.
- The Mission Control finds an available module ID and sends an ASSIGN_MODULE_ID
  message.
- The sign-in procedure ends here. The joining module will use this ID for quicker communication
  until it signs off (typically by power off).

### Sign-In Procedure for Mission Control
When the Mission Control joins the network for the first time, it should run
the sign-up procedure as follows:

- The Mission Control sends SIGN_IN message with its ID (0x700).
- Each individual modules should reply by SIGN_IN if its ID has not been
  assigned yet, or NOTIFY_ID if its ID is assigned already.
- The Mission Control must respond to SIGN_IN messages if any.
- The sign-in procedure ends here. The Mission Control should know IDs of all
  modules in the network then.

## Configuration Data Scheme

A TLV based scheme is used to exchange structured data between two modules over the CAN network.
A data element is called *field*. A field is identified by *field ID*. A field ID is associated
with a pre-defined name. Data type of a field is also pre-defined. Field names are not exchanged
ove the CAN network. Individual nodes must save the ID to name mapping if necessary.
Dynamic type ID allocation is not supported yet.

But a receiver does not necessarily have to understand the semantics of fields thanks
to the TLV structure.
For example, the Mission Control does not have to know a module's configuration to save it to a
storage media.

### Basic structure
In C syntax, a configuration data field consists of following members.

```
struct DataField {
  uint8_t field_id;
  uint8_t length;
  uint8_t data[N];
};
```

where 0 < N < 256

We assume data longer than 255 byte is not necessary in an Analog3 environment.
To exchange the data, the fields are put to a byte array in the described order above, i.e.,

```
| field_id | length | data[0] | data[1] | ... | data[N-1] |
```

In case of sending multiple fields, wrap them into a Chunk:

```
struct Chunk {
  uint8_t num_fields;
  struct DataField fields[];
}
```

To exchange the data, the fields are put to a byte array, i.e.,

```
| num_fields | field_id_0 | length_0 | data_0[0] | ... | data_0[N-1] | field_id_1 | ... |
```

Notes:

- All fields are "flat". Nested structure is not allowed intentionally
  to avoid modules doing a complex job to parse
- A Chunk can contain multiple fields with the same ID. Treat them as a
  list of fields. But the data structure is still "flat" in the Chunk.

### Data Transmission Protocol
A CAN message contains only eight bytes at most. The sending data has to
be broken into multiple messages to transmit using a CAN network.

A data exchange should be executed with following procedure:

1. The requester initiates the exchange by sending a REQUEST message.
2. The data sender encodes the replying data and send the stream of the encoded data.
   A stream consists of one or more CAN messages. Each message should contain 7 bytes
   of the streaming data, which is called a *segment*. Send the first segment.
3. The requester handles the reply and send a data send a CONTINUE message
   if there are more segments to receive.
4. The data sender sends the next segment.
5. Repeat 3 and 4 until all segments are transmitted.

A data REQUEST and CONTINUE messages should consists of following data bytes:

```
0: opcode
1: remote ID
```

A data transmit message should consist of following data bytes:

```
0: opcode
1: data
2: data
3: data
4: data
5: data
6: data
7: data
```

### Data Types

#### U8
Unsigned 8-bit integer. Encoded stream should be:

```
uint8_t value;
| value |
```

#### U16
Unsigned 16-bit integer. Encoded stream should be:

```
uint16_t value;
| (value >> 8) & 0xff | value & 0xff |
```

#### U32
Unsigned 32-bit integer. Encoded stream should be:

```
uint32_t value;
| (value >> 24) & 0xff | (value >> 16) & 0xff | (value >> 8) & 0xff | value & 0xff |
```

#### String
A string with length up to 255. Encoded stream should be as the following.
The string body does NOT contain terminating null.

```
| length | body[0] | body[1] | ... | body[length - 1] |
```

### Field Definitions

| Field ID | Field Name | Data Type | Description |
| -------- | ---------- | --------- | ----------- |
| 1        | Name       | String    | Module name |