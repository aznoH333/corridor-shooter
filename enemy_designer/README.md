# Enemy Designer

Desktop tool for authoring reusable enemy parts and composing them into enemies for corridor-shooter.

## Requirements

- Python 3.10+
- Linux or macOS

## Setup

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Or system-wide / user install:

```bash
pip3 install --user -r requirements.txt
```

## Run

```bash
PYTHONPATH=. python3 -m src.main
```

## Layout

See `AGENTS.md` for the domain model, file formats, and UX requirements.
