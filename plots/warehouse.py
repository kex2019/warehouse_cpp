"""Utils for data exploration."""
import pandas as pd
import numpy as np


def read(data_str: str, type="standard") -> pd.DataFrame:
    """Read content of file to dataframe."""
    readers = {"standard": _read_standard, "work": _read_work}

    return readers[type](data_str)


def _read_work(data_str: str):
    """Read content of file to dataframe."""
    # ids = []
    group_ids = []
    num_robots = []
    data = []

    for line in data_str.split("\n"):
        if line == "":
            continue
        _, gid, r, *robots = line.split(",")

        robots = str((list(map(float, robots))))

        group_ids.append(gid)
        num_robots.append(float(r))
        data.append(robots)

    return pd.DataFrame({
        "group": group_ids,
        "robots": num_robots,
        "data": data
    })


def _read_standard(data_str: str):
    """Read content of file to dataframe."""
    # ids = []
    scores = []
    group_ids = []
    # robots = []
    robots = []
    capacity = []
    packages = []
    seed = []
    computation = []

    for line in data_str.split("\n"):
        if line == "":
            continue
        _, gid, s, u, _, c, p, sd, cp = line.split(",")

        scores.append(float(s))
        group_ids.append(gid)
        robots.append(float(u))
        capacity.append(float(c))
        packages.append(float(p))
        seed.append(float(sd))
        computation.append(float(cp))

    return pd.DataFrame({
        "scores": scores,
        "group": group_ids,
        "robots": robots,
        "capacity": capacity,
        "packages": packages,
        "seed": seed,
        "computation": computation
    })
