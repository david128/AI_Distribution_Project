#include "DistributionCentre.h"

DistributionCentre::DistributionCentre()
{
}

DistributionCentre::DistributionCentre(float max, float startingStock, Lorry l, int numOfLorries)
{
	for (int i = 0; i < numOfLorries; i++)
	{
		lorries.push_back(l);
	}
	
	
	maxStock = max;
	currentStock = startingStock;

}
