"""Utils for data exploration."""
import pandas as pd


def read(data_str: str) -> pd.DataFrame:
    """Read content of file to dataframe."""
    scores = []
    robots = []
    capacity = []
    packages = []
    seed = []
    computation = []

    for line in data_str.split("\n"):
        if line == "":
            continue
        s, r, c, p, sd, cp = line.split(",")

        scores.append(float(s))
        robots.append(float(r))
        capacity.append(float(c))
        packages.append(float(p))
        seed.append(float(sd))
        computation.append(float(cp))

    return pd.DataFrame({
        "scores": scores,
        "robots": robots,
        "capacity": capacity,
        "packages": packages,
        "seed": seed,
        "computation": computation
    })
