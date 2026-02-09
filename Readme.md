# Compile-Time Ray Tracer (Compiler-Error Art)

This project renders a simple 3D scene **entirely at compile time** and **intentionally fails compilation** to display the resulting ASCII art inside compiler error messages. There is no successful executable; the “output” is the error log.

## What It Does

- Builds a constexpr ray tracer (spheres, lighting, shadows) in C++20.
- Converts the rendered image to ASCII (80x40).
- Forces compilation to fail so the ASCII art appears in errors.
- Provides a post-processor to clean the error stream into readable ASCII.

## Files

- `raytracer.cpp` — constexpr ray tracer and error emitter.
- `tools/clean_errors.py` — extracts ASCII art from `LineTag<...>` errors.
- `tools/render.sh` — builds, saves raw errors, and writes cleaned image.

## Requirements

- Clang (C++20). GCC is not recommended for the error-emission approach used here.
- Python 3 (for the post-processor).

## Build / Render

### 1) Raw compiler errors (ASCII encoded as types)

```
clang++ -std=c++20 -fconstexpr-steps=10000000 -ferror-limit=0 raytracer.cpp
```

This prints a wall of errors containing `LineTag<...>` type instantiations. Each `LineTag<...>` encodes one line of the ASCII image.

### 2) Clean ASCII output

```
clang++ -std=c++20 -fconstexpr-steps=10000000 -ferror-limit=0 raytracer.cpp 2>&1 | tools/clean_errors.py
```

### Filtered output (example)

```
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::-==:::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::--==+++*::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::.::--==++****::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::: .::--==++****#:::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::  .::--==+++***##:::::::::::::::::**:::::::::::::::::::::::::::::::
-----::::::::  .::---==++****#*:::::::::::::-==++*##:::::::::::::::::::::::-----
------------   ..::--===++*****:::::::::::.::-==+*#*#::::::::::::---------------
------------   ..:::--===++*****::::::::::..::--=+%%**--------------------------
------------    ..::---=++*##%*+---------  ..::--==++**---------**####%---------
------------     ..:.:-=++*##%%@--------    ...::--==++-------====+%@%**#-------
------------     .. .:--=++*##%%%-------      ..::---==+-----:::::---===++------
------------      . .::-==+**%%%%-------       ...::---=----    ...::::--==-----
-------------        .:--==+*####%------         ..:::------         ...::------
--------------       ..:--==++**##------          ...:::---              ..-----
--------------        .::--==++***....::            ...-----               -----
-----------...         .::--==++++:::::::              :::::               -----
                        ..::--====                    -------             ::::::
                         ..::-----:                   --------            ------
::::::::::::::            ...:::::-------          ------------         --------
:::::::::                    ...------------------------           -------------
:::::::                       :---------------------------             ---------
::::::::                ::::::--------------------------------           -------
:::::::::::::::::::::::::::::---------------------------------------------------
::::::::::::::::::::::::::::----------------------------------------------------
:::::::::::::::::::::::::::-----------------------------------------------------
:::::::::::::::::::::::::::-----------------------------------------------------
::::::::::::::::::::::::::------------------------------------------------------
:::::::::::::::::::::::::-------------------------------------------------------
:::::::::::::::::::::::::-------------------------------------------------------
::::::::::::::::::::::::--------------------------------------------------------
::::::::::::::::::::::::--------------------------------------------------------
:::::::::::::::::::::::---------------------------------------------------------
```

### 3) One-command render (raw + cleaned)

```
./tools/render.sh
```

This writes:
- `raw.txt` — full compiler error output
- `image.txt` — cleaned ASCII art

You can pass custom filenames:

```
./tools/render.sh raw_output.txt image_output.txt
```

## How the Output Works

C++20 requires `static_assert` messages to be string literals. Because we need a **computed** string, we encode each output line as a **type** instead:

- Each line becomes `LineTag<' ','.',':',...>`.
- `static_assert` fires on a dependent type so Clang prints the full type.
- The post-processor strips punctuation and reconstructs the ASCII art.

## Scene Details

The scene is a small collection of spheres with a single point light:

- Ground sphere for the floor
- Multiple spheres with different sizes and specular values
- Shadows for depth perception

You can tweak the scene in `raytracer.cpp`:

- `spheres` array (positions, radius, albedo, specular)
- `light_pos` and `light_color`
- camera / viewport parameters

## Tuning Compile-Time Limits

If you hit constexpr step limits, increase the steps:

```
clang++ -std=c++20 -fconstexpr-steps=20000000 -ferror-limit=0 raytracer.cpp
```

## Notes

- This project intentionally **fails to compile**.
- The ASCII output is the error log (or the filtered output via `clean_errors.py`).
- For presentations/demos, use `raw.txt` and `image.txt` side by side.

## License

Unspecified. Add a LICENSE file if you intend to publish.
