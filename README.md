# libpiano-nx

Pandora's client API as a static library for Nintendo Switch homebrew, split out of
[pianobar](https://codeberg.org/purplesym/pianobar)'s `src/libpiano` with its history preserved
(`git subtree split`).

Built for [pianobar-nx](https://github.com/nx-mod/pianobar-nx), but it depends on nothing of that
player: it speaks the Pandora JSON API and leaves transport, playback and UI to the caller.

## What it does

- Builds and parses every Pandora request: login, station list, playlist fetch, feedback, bookmarks,
  station create/rename/delete, explanations, search.
- Blowfish encrypt/decrypt of request bodies (`source/crypt.c`), with a gcrypt-shaped shim
  (`source/gcrypt_compat.c`) so no libgcrypt is needed on Horizon.
- No sockets of its own. `PianoRequest` hands you a URL and a body; you send it and hand the response
  back to `PianoResponse`.

## Building

```sh
make                 # devkitA64 + libnx from $DEVKITPRO
bash build.sh        # the same, inside the devkitPro MSYS2 shell
```

The result is `lib/libpiano-nx.a`, with headers in `include/`. `build.sh` points `LIBNX` at a flat
`../libnx` checkout, matching the other projects in the switch-cfw tree; plain `make` uses devkitPro's
stock libnx.

## Using it

```c
#include <piano.h>

PianoHandle_t ph;
PianoInit(&ph, "");                       /* partner credentials go in the request */
PianoRequestDataLogin_t login = { .user = "...", .password = "...", .step = 0 };
PianoRequest_t request = { .type = PIANO_REQUEST_LOGIN, .data = &login };
PianoRequest(&ph, &request, request.type);
/* send request.postData to request.urlPath yourself, then: */
PianoResponse(&ph, &request);
PianoDestroy(&ph);
```

`config.h` guards `PACKAGE`/`VERSION` so a consumer that defines its own first (pianobar-nx does) will
not warn.

## License

GPL-3.0, as pianobar is. Not affiliated with Pandora Media.
