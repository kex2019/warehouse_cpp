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
        s, r, c, p, sd, cp = line.split(",")

        scores.append(s)
        robots.append(r)
        capacity.append(c)
        packages.append(p)
        seed.append(sd)
        computation.append(cp)

    return pd.DataFrame({
        "scores": scores,
        "robots": robots,
        "capacity": capacity,
        "packages": packages,
        "seed": seed,
        "computation": computation
    })
