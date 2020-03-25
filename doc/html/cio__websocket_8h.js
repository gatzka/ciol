var cio__websocket_8h =
[
    [ "cio_websocket_write_job", "structcio__websocket__write__job.html", "structcio__websocket__write__job" ],
    [ "cio_response_buffer", "structcio__response__buffer.html", "structcio__response__buffer" ],
    [ "cio_websocket_private", "structcio__websocket__private.html", "structcio__websocket__private" ],
    [ "cio_websocket", "structcio__websocket.html", "structcio__websocket" ],
    [ "cio_websocket_close_hook", "cio__websocket_8h.html#aca914fcd6af51949d43432f155c61e19", null ],
    [ "cio_websocket_on_connect", "cio__websocket_8h.html#a47f4e2befc8d1d0ef6478e7bb7d1bd60", null ],
    [ "cio_websocket_read_handler", "cio__websocket_8h.html#afa44f1ffa8cef33d0367d3e6a052241a", null ],
    [ "cio_websocket_write_handler", "cio__websocket_8h.html#a708dd79eefd7e6609cae39f515174e3b", null ],
    [ "CIO_WEBSOCKET_SMALL_FRAME_SIZE", "cio__websocket_8h.html#abc6126af1d45847bc59afa0aa3216b04aab5801980f3504f11ccabd66e879281b", null ],
    [ "CIO_WEBSOCKET_MAX_HEADER_SIZE", "cio__websocket_8h.html#adc29c2ff13d900c2f185ee95427fb06ca66accc0b17c00cd228ce2e5d78e609bf", null ],
    [ "cio_websocket_frame_type", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9", [
      [ "CIO_WEBSOCKET_CONTINUATION_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9a268d3844949375e76864185036973bd9", null ],
      [ "CIO_WEBSOCKET_TEXT_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9a4698fc6e497c243a27b44361cf407090", null ],
      [ "CIO_WEBSOCKET_BINARY_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9a35933dcf2a9173b8b2c05a87113f2ebb", null ],
      [ "CIO_WEBSOCKET_CLOSE_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9a2c1fb6190c895d3c5d1c22310af91a5d", null ],
      [ "CIO_WEBSOCKET_PING_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9a42de48db574a3eb78ee662c4683f4ed3", null ],
      [ "CIO_WEBSOCKET_PONG_FRAME", "cio__websocket_8h.html#af33fb069400eeffde11ab324bf2245e9ac89c2941151a6830f7cf69729ee9693c", null ]
    ] ],
    [ "cio_websocket_status_code", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701", [
      [ "CIO_WEBSOCKET_CLOSE_NORMAL", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a82f24208e6292926e8e02352913d1e09", null ],
      [ "CIO_WEBSOCKET_CLOSE_GOING_AWAY", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a8ee520b364af620ff44e60a09c393b97", null ],
      [ "CIO_WEBSOCKET_CLOSE_PROTOCOL_ERROR", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a5a7495da78c928168a5746962e7e595c", null ],
      [ "CIO_WEBSOCKET_CLOSE_UNSUPPORTED", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a0c5ed5627c015f6e7b81c3fc74d7c28e", null ],
      [ "CIO_WEBSOCKET_CLOSE_NO_STATUS", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701aafc6ff9d1c633323a43641cc8b25dd9a", null ],
      [ "CIO_WEBSOCKET_CLOSE_UNSUPPORTED_DATA", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701ac561a452a68e836193daadbdf1db5c10", null ],
      [ "CIO_WEBSOCKET_CLOSE_POLICY_VIOLATION", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a0ac94d16eff35891a1251a40004171b6", null ],
      [ "CIO_WEBSOCKET_CLOSE_TOO_LARGE", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a2d635168ecf8e202026506cf38d00732", null ],
      [ "CIO_WEBSOCKET_CLOSE_MISSING_EXTENSION", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a4bb8a74d7c07bacc386e8f849b427434", null ],
      [ "CIO_WEBSOCKET_CLOSE_INTERNAL_ERROR", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a8dd8f47511214b8ed8b6901de272e845", null ],
      [ "CIO_WEBSOCKET_CLOSE_SERVICE_RESTART", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a011dbf3875199a1614ef3a81220dc088", null ],
      [ "CIO_WEBSOCKET_CLOSE_TRY_AGAIN_LATER", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a4fe538993d22d7dbceb83f2cb17937e8", null ],
      [ "CIO_WEBSOCKET_CLOSE_TLS_HANDSHAKE", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a89041f290dedaca9d7b32b434f57063b", null ],
      [ "CIO_WEBSOCKET_CLOSE_RESERVED_LOWER_BOUND", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701a174e73826b58a9e38b3d84aad7855638", null ],
      [ "CIO_WEBSOCKET_CLOSE_RESERVED_UPPER_BOUND", "cio__websocket_8h.html#aa24c9894ba2ee9dc5b0ebbc1c8809701af96f1ba0788e17c59249f12a74defd2d", null ]
    ] ],
    [ "cio_websocket_close", "cio__websocket_8h.html#a75cfee481129f49e25042f683a4e5031", null ],
    [ "cio_websocket_init", "cio__websocket_8h.html#acf943032bcd768ba38f15fd37655f70d", null ],
    [ "cio_websocket_read_message", "cio__websocket_8h.html#ab07aad4d396c7241d8354531ee2d9dcf", null ],
    [ "cio_websocket_set_on_control_cb", "cio__websocket_8h.html#a00cabb7944feacb0549a483a8b5f6e81", null ],
    [ "cio_websocket_set_on_error_cb", "cio__websocket_8h.html#ab59912afcbcc5f9250106b228b7434fe", null ],
    [ "cio_websocket_write_message_continuation_chunk", "cio__websocket_8h.html#a60b9c1d084272f594c7e1bb4476ca724", null ],
    [ "cio_websocket_write_message_first_chunk", "cio__websocket_8h.html#aceaadce9f1f806d7ab93859e74bebce7", null ],
    [ "cio_websocket_write_ping", "cio__websocket_8h.html#a53626782f6d0f199ae8128f27f7e11b1", null ],
    [ "cio_websocket_write_pong", "cio__websocket_8h.html#addfd019deec62389b8cd23ed3f95b9ef", null ]
];