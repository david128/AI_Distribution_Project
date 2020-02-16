#include "DistributionCentre.h"

DistributionCentre::DistributionCentre()
{
}

DistributionCentre::DistributionCentre(float max, Lorry l)
{
	lorries[0] = l;
	lorries[1] = l;
	maxStock = max;

}
