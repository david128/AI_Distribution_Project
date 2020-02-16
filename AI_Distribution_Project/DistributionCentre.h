#pragma once

struct Lorry
{
	float daysUntilReturn;
	float carryLimit;
};

class DistributionCentre
{
private:
	float maxStock;

public:

	DistributionCentre();

	DistributionCentre(float max, Lorry l);



	Lorry lorries[2];

	



};

