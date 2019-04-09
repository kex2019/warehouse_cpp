import numpy as np
import matplotlib.pyplot as plt
import cv2

XS = {
    "aisles": 2,
    "aisleWidth": 2,
    "crossAiles": 2,
    "crossAilesWidth": 2,
    "shelfHeight": 10,
    "packages": 20,
}

S = {
    "aisles": 4,
    "aisleWidth": 2,
    "crossAiles": 2,
    "crossAilesWidth": 2,
    "shelfHeight": 20,
    "packages": 40,
}

M = {
    "aisles": 8,
    "aisleWidth": 2,
    "crossAiles": 2,
    "crossAilesWidth": 2,
    "shelfHeight": 20,
    "packages": 80,
}

L = {
    "aisles": 16,
    "aisleWidth": 3,
    "crossAiles": 3,
    "crossAilesWidth": 3,
    "shelfHeight": 20,
    "packages": 160,
}

XL = {
    "aisles": 32,
    "aisleWidth": 3,
    "crossAiles": 3,
    "crossAilesWidth": 3,
    "shelfHeight": 20,
    "packages": 320,
}

XXL = {
    "aisles": 64,
    "aisleWidth": 3,
    "crossAiles": 3,
    "crossAilesWidth": 3,
    "shelfHeight": 20,
    "packages": 640
}

XXXL = {
    "aisles": 100,
    "aisleWidth": 3,
    "crossAiles": 4,
    "crossAilesWidth": 3,
    "shelfHeight": 20,
    "packages": 800
}


def viz_wh(info: "WarehouseInfo", name, show=True):

    if info["crossAilesWidth"] < 2:
        info["crossAilesWidth"] = 2

    info["aisleWidth"] += 2

    shelfX = [0] * (info["aisles"] * 2)
    for i in range(info["aisles"]):
        shelfX[2 * i] = i * info["aisleWidth"]
        shelfX[2 * i + 1] = i * info["aisleWidth"] + info["aisleWidth"] - 1

    maxX = info["aisleWidth"] * (info["aisles"] - 1) + info["aisleWidth"]
    shelfY = [0] * (info["crossAiles"] + 1)
    for i in range(info["crossAiles"] + 1):
        shelfY[i] = info["shelfHeight"] * i + info["crossAilesWidth"] * (i + 1)

    maxY = info["shelfHeight"] * (
        info["crossAiles"]) + info["crossAilesWidth"] * (
            info["crossAiles"] + 1
        ) + info["shelfHeight"] + info["crossAilesWidth"]

    walkable = [[True for _ in range(maxX)] for _ in range(maxY)]
    for yidx in range(info["crossAiles"] + 1):
        for xidx in range(info["aisles"] * 2):
            for i in range(info["shelfHeight"]):
                walkable[shelfY[yidx] + i][shelfX[xidx]] = False

    shelfColor = np.array([0, 0, 0])
    aisleColor = np.array([1, 1, 1])

    image = np.zeros((maxY, maxX, 3), dtype=np.float32)

    for i in range(maxY):
        for j in range(maxX):
            image[i][j] = aisleColor if walkable[i][j] else shelfColor

    ratio = maxX / maxX
    imY, imX = min(100 * maxY, 800), min(100 * maxX * ratio, 800 * ratio)

    imResize = cv2.resize(
        image, (int(imX), int(imY)), interpolation=cv2.INTER_NEAREST)

    plt.axis('off')
    plt.margins(0, 0)
    plt.title(name)
    plt.imshow(imResize)
    plt.savefig(
        "plots/outputs/%s.pdf" % name, bbox_inches='tight', pad_inches=0)
    plt.show()


viz_wh(XS, name="XS", show=True)
viz_wh(S, name="S", show=True)
viz_wh(M, name="M", show=True)
viz_wh(L, name="L", show=True)
viz_wh(XL, name="XL", show=True)
viz_wh(XXL, name="XXL", show=True)
viz_wh(XXXL, name="XXXL", show=True)
