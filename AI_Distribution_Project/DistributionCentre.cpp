#include "DistributionCentre.h"

DistributionCentre::DistributionCentre()
{
}

DistributionCentre::DistributionCentre(float max, float startingStock, Lorry l)
{
	lorries[0] = l;
	lorries[1] = l;
	maxStock = max;
	currentStock = startingStock;

}
