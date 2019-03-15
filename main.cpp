#include <iostream>
#include "warehouse.h"

int main() {
    WarehouseInfo info;
    info.aisles = 3;
    info.aisleWidth = 2;
    info.crossAiles = 1;
    info.crossAilesWidth = 2;
    info.shelfHeight = 5;
    info.packages = 10;

    Warehouse warehouse = generateRandomWarehouse(info);
    std::cout << warehouse.to_string() << endl;
}