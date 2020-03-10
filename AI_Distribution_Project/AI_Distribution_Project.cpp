#include <iostream>
#include "Seller.h"
#include "DistributionCentre.h"
#include <string>
#include <ctime>
#include <fstream>
#include "fl/Headers.h"

using namespace std;
using namespace fl;

//bool for which system to use
bool fuzzy;

//sellers vector
vector<Seller> sellers;

//holds information about distribution and lorries
DistributionCentre dist;

//num of sellers and lorries
int numOfSellers = 0;
int numOfLorries = 0;

//sales missed for each seller
vector<float> salesMissed;

//values to compare to in rules
float stockLow = 50.0f;
float farDist = 3.0f;
float highSPD = 10.0f;


//fuzzy variables for fuzzy lite engine
Engine* engine = new Engine;
InputVariable* Stock = new InputVariable;
InputVariable* Distance = new InputVariable;
InputVariable* SalesPerDay = new InputVariable;
InputVariable* DeliveryOnItsWay = new InputVariable;
OutputVariable* urgency = new OutputVariable;
RuleBlock* ruleBlock = new RuleBlock;


//data for rbs
struct DecisionData
{
	float stockPer;
	float dist;
	float spd;
	bool deliveryOnItsWay;
};

//data for fuzzy system
struct FuzzyDecisionData
{
	float stockPer;
	float dist;
	float spd;
	float deliveryOnItsWay;
};


//read in files with data for sellers and lorries
void ReadInFiles()
{
	//files
	std::ifstream sellersFile("sellers.txt");
	std::ifstream lorryFile("lorries.txt");
	
	float sm = 0; //starting value for sales missed
	string a, b, c; //temp storage
	
	while(sellersFile >> a >> b >> c) //read first 3 seperated values until end of file
	{

		sellers.push_back(Seller(stof(a), stof(b), stof(c))); //add to list seller with values read 
		salesMissed.push_back(sm); //add sales missed
	}


	Lorry lorry; // create lorry 	

	lorryFile >> lorry.carryLimit >> numOfLorries; //read from lorry file, carry limit and number of lorries

	//set default values
	lorry.daysUntilArrive = 0.0f;
	lorry.carrying = 0.0f;
	lorry.phase = IDLE;
	dist = DistributionCentre(500.0f, 300.0f, lorry, numOfLorries);//create distribution centre


	numOfSellers = sellers.size(); //num of sellers is the size of sellers vector
}



// start process of moving from dist centre to seller 
void MoveStock(int sellerID, int LorryID)
{

	
	dist.lorries[LorryID].carrying = dist.lorries[LorryID].carryLimit; //carry carry limit amount of stock


	dist.lorries[LorryID].daysUntilArrive = sellers[sellerID].GetTravelTime(); //set days until arrival to full travel time
	dist.lorries[LorryID].sellerID = sellerID; //set sellerID to ID traveling to
	dist.lorries[LorryID].phase = MOVING; //set phase to moving

	std::cout << fixed << setprecision(0) << "Truck "  << (LorryID + 1) << " has been dispatched to shop " << (dist.lorries[LorryID].sellerID + 1) << " carrying  " << (dist.lorries[LorryID].carrying) << " stock and will arrive in " << (dist.lorries[LorryID].daysUntilArrive) << " days\n"; //output message summarising what is happening
}


void DeliverStock(int sellerID, int LorryID) //when arriving add to stock
{
	//check if new stock will be higher than max stock, if higher then set stock to max stock, else add carrying to current stock
	float newStock = sellers[sellerID].GetStock() + dist.lorries[LorryID].carrying; 
	if (newStock <= sellers[sellerID].GetMaxStock())
	{
		sellers[sellerID].AddStock(dist.lorries[LorryID].carrying);
		std::cout << fixed << setprecision(0) << "Truck "  << (LorryID + 1) << " has delivered " <<(dist.lorries[LorryID].carrying ) <<  " to shop "<< (dist.lorries[LorryID].sellerID + 1) << "\n";
		dist.lorries[LorryID].carrying = 0;

	}
	else
	{
		
		std::cout << fixed << setprecision(0) << "Truck " << LorryID + 1 << " has delivered " <<((sellers[sellerID].GetMaxStock() - sellers[sellerID].GetStock())) << " to shop " << (dist.lorries[LorryID].sellerID + 1) << "\n";
		sellers[sellerID].SetStock(sellers[sellerID].GetMaxStock());
		dist.lorries[LorryID].carrying = 0;
	}

	
}

void Sell(int sellerID)
{
	cout << fixed << setprecision(0) << "Shop " << (sellerID+1) << " started with " << (sellers[sellerID].GetStock()); //start of message

	if (sellers[sellerID].GetStock() >= sellers[sellerID].GetSalesPerDay()) //if stock is >= to spd then no sales are missed, else sales have been missed
	{
		cout << fixed << setprecision(0) << " stock, sold " << (sellers[sellerID].GetSalesPerDay()); //output stock sold
		sellers[sellerID].SellStock(sellers[sellerID].GetSalesPerDay());
		cout << fixed << setprecision(0) << " stock and ended with " <<  (sellers[sellerID].GetStock()) << " stock, missing 0 sales\n"; //end of message with no sales missed
	}
	else
	{
		cout << fixed << setprecision(0) << " stock, sold " << (sellers[sellerID].GetStock()); //output stock sold
		
		//end of message with no sales missed
		int sm = sellers[sellerID].GetSalesPerDay() - sellers[sellerID].GetStock();
		salesMissed[sellerID] += sm;
		sellers[sellerID].SellStock(sellers[sellerID].GetStock()); //sell stock
		cout << fixed << setprecision(0) << " stock ended with " << (sellers[sellerID].GetStock()) << " stock, missing "+ to_string(sm ) << " sales\n"; //end of message with sales missed value
	}
	
}

DecisionData GetDecisionData(int sellerID)// get rbs data
{
	DecisionData decisionData;

	//get stock full percentage, distance, sales per day
	decisionData.stockPer = sellers[sellerID].GetPercentageFull(); 
	decisionData.dist = sellers[sellerID].GetTravelTime();
	decisionData.spd = sellers[sellerID].GetSalesPerDay();

	//check if delivery ison its way to seller
	decisionData.deliveryOnItsWay = false;
	for (int i = 0; i < numOfLorries; i++)
	{
		if (dist.lorries[i].phase == MOVING && dist.lorries[i].sellerID == sellerID)
		{
			decisionData.deliveryOnItsWay = true;
		}
	}
	
	return decisionData;
}

FuzzyDecisionData GetFuzzyDecisionData(int sellerID) // get fuzzy system data
{
	FuzzyDecisionData decisionData;
	
	//get stock full percentage, distance, sales per day
	decisionData.stockPer = sellers[sellerID].GetPercentageFull();
	decisionData.dist = sellers[sellerID].GetTravelTime();
	decisionData.spd = sellers[sellerID].GetSalesPerDay();

	//check if delivery is on its way, if none return 7
	decisionData.deliveryOnItsWay = 7;
	
	//if 1 return days until arrival, if multiple return lowest days until arrival / amount of deliveries on the way
	float num = 0;
	for (int i = 0; i < numOfLorries; i++)
	{
		
		if (dist.lorries[i].phase == MOVING && dist.lorries[i].sellerID == sellerID )
		{
			num++;
			if (dist.lorries[i].daysUntilArrive < decisionData.deliveryOnItsWay)
			{
				decisionData.deliveryOnItsWay = dist.lorries[i].daysUntilArrive;
			}
			
		}
	}

	if (num != 0)
	{
		decisionData.deliveryOnItsWay = decisionData.deliveryOnItsWay / num;
	}


	return decisionData;
}

void SetUpFuzzyLogic()
{
	//using system created in matlab fuzzy toolbox exported as fis
	//imported to fl library and then exported into the following c++ code

	//Code automatically generated with fuzzylite 6.0.

	
	engine->setName("distributionFuzzy1");
	engine->setDescription("");

	
	Stock->setName("Stock");
	Stock->setDescription("");
	Stock->setEnabled(true);
	Stock->setRange(0.000, 100.000);
	Stock->setLockValueInRange(false);
	Stock->addTerm(new Triangle("Empty", -25.000, 0.000, 15.000));
	Stock->addTerm(new Triangle("Low", 10.300, 25.600, 50.600));
	Stock->addTerm(new Triangle("Medium", 30.000, 50.000, 70.000));
	Stock->addTerm(new Triangle("WellStocked", 50.000, 75.000, 90.000));
	Stock->addTerm(new Triangle("Full", 85.000, 100.000, 125.000));
	engine->addInputVariable(Stock);

	
	Distance->setName("Distance");
	Distance->setDescription("");
	Distance->setEnabled(true);
	Distance->setRange(0.000, 6.000);
	Distance->setLockValueInRange(false);
	Distance->addTerm(new Triangle("Close", -3.000, 0.000, 3.000));
	Distance->addTerm(new Triangle("MediumDistance", 1.500, 3.000, 4.500));
	Distance->addTerm(new Triangle("Far", 3.000, 6.000, 9.000));
	engine->addInputVariable(Distance);

	
	SalesPerDay->setName("SalesPerDay");
	SalesPerDay->setDescription("");
	SalesPerDay->setEnabled(true);
	SalesPerDay->setRange(0.000, 20.000);
	SalesPerDay->setLockValueInRange(false);
	SalesPerDay->addTerm(new Triangle("Low", -8.000, 0.000, 8.000));
	SalesPerDay->addTerm(new Triangle("Moderate", 4.000, 10.000, 16.011));
	SalesPerDay->addTerm(new Triangle("High", 12.000, 20.000, 28.000));
	engine->addInputVariable(SalesPerDay);

	DeliveryOnItsWay->setName("DeliveryOnItsWay");
	DeliveryOnItsWay->setDescription("");
	DeliveryOnItsWay->setEnabled(true);
	DeliveryOnItsWay->setRange(0.000, 7.000);
	DeliveryOnItsWay->setLockValueInRange(false);
	DeliveryOnItsWay->addTerm(new Triangle("Soon", -2.500, 0.000, 2.500));
	DeliveryOnItsWay->addTerm(new Triangle("OnItsWay", 1.500, 2.500, 5.000));
	DeliveryOnItsWay->addTerm(new Triangle("NotSoon", 4.000, 5.000, 6.000));
	DeliveryOnItsWay->addTerm(new Triangle("NoDelivery", 6.000, 7.000, 9.350));
	engine->addInputVariable(DeliveryOnItsWay);

	urgency->setName("urgency");
	urgency->setDescription("");
	urgency->setEnabled(true);
	urgency->setRange(0.000, 1.000);
	urgency->setLockValueInRange(false);
	urgency->setAggregation(new Maximum);
	urgency->setDefuzzifier(new Centroid(100));
	urgency->setDefaultValue(fl::nan);
	urgency->setLockPreviousValue(false);
	urgency->addTerm(new Triangle("VeryLow", -0.250, 0.000, 0.250));
	urgency->addTerm(new Triangle("Low", 0.000, 0.250, 0.500));
	urgency->addTerm(new Triangle("Average", 0.250, 0.500, 0.750));
	urgency->addTerm(new Triangle("High", 0.500, 0.750, 1.000));
	urgency->addTerm(new Triangle("VeryHigh", 0.750, 1.000, 1.250));
	engine->addOutputVariable(urgency);

	ruleBlock->setName("");
	ruleBlock->setDescription("");
	ruleBlock->setEnabled(true);
	ruleBlock->setConjunction(new Minimum);
	ruleBlock->setDisjunction(new Maximum);
	ruleBlock->setImplication(new Minimum);
	ruleBlock->setActivation(new General);
	ruleBlock->addRule(Rule::parse("if Stock is Empty then urgency is VeryHigh", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Low then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Medium then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if Stock is WellStocked then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Full then urgency is VeryLow", engine));
	ruleBlock->addRule(Rule::parse("if Distance is Close then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if Distance is MediumDistance then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if Distance is Far then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is Low then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is Moderate then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is High then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is Soon then urgency is VeryLow", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is OnItsWay then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NotSoon then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NoDelivery then urgency is High", engine));
	engine->addRuleBlock(ruleBlock);


	
}

void runFuzzyLogic(int lorryID)//fuzzy system
{

	vector<float> sellerUrgency;// urgency of each seller

	//check each seller urgency value
	for (int i = 0; i < numOfSellers; i++)
	{
		FuzzyDecisionData d = GetFuzzyDecisionData(i); //get data 

		//set values from data
		Stock->setValue(d.stockPer);
		Distance->setValue(d.dist);
		SalesPerDay->setValue(d.spd);
		DeliveryOnItsWay->setValue(d.deliveryOnItsWay);

		engine->process(); //process data in fuzzy engine


		sellerUrgency.push_back(urgency->getValue()); // add urgency
		
	}
	
	//find most urgent
	int mostUrgent = 0;
	for (int i = 1; i < numOfSellers; i++)
	{
		if (sellerUrgency[mostUrgent] < sellerUrgency[i])
		{
			mostUrgent = i;
		}
	}

	//move to most urgent
	MoveStock(mostUrgent, lorryID);
	
}

void RuleBased(int lorryID)
{
	vector<float> urgency = { 0 };
	//check sellers and give each a value for how urgent they need a delivery based on rules
	for (int i = 0; i < numOfSellers; i++)
	{
		float thisUrgency = 0;
		urgency.push_back(thisUrgency);
		DecisionData d = GetDecisionData(i);
		if (d.stockPer < stockLow)
		{
			urgency[i] += 3;
		}
		else
		{
			urgency[i] -= 3;
		}

		if (d.dist > farDist)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}

		if (d.spd > highSPD)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}

		if (d.deliveryOnItsWay == false)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}
	}

	//find most urgent
	int mostUrgent = 0;
	for (int i = 1; i < numOfSellers; i++)
	{
		if (urgency[mostUrgent] < urgency[i])
		{
			
			mostUrgent = i;
		}
	}

	//move stock to most urgent seller
	MoveStock(mostUrgent, lorryID);
}

//placeholder for AI which randomly chooses a seller
void RandomPlaceholder(int lorryID)
{
	
	MoveStock((rand() % numOfSellers),lorryID);
	
}

void CheckLorries()
{
	for (int i = 0; i < numOfLorries; i++)
	{
		if (dist.lorries[i].phase == IDLE) //if a lorry is idle then make a decision on where to send it
		{
			//based on input choose system		
			if (fuzzy == true)
			{
				runFuzzyLogic(i);
			}
			else
			{
				RuleBased(i);
			}

		}
		else if (dist.lorries[i].phase == MOVING)
		{
			//if moving, remove one from days until arrival
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << fixed << setprecision(0) << "Truck  " << (i+1) << " is on its way to shop " << (dist.lorries[i].sellerID+1) << " will arrive in " << (dist.lorries[i].daysUntilArrive)  << " days\n";
			}
			else
			{
				//if arrived set phase to arrived
				dist.lorries[i].daysUntilArrive -= 1;
				DeliverStock(dist.lorries[i].sellerID,i);
				dist.lorries[i].phase = ARRIVED;
			}
		}
		else if (dist.lorries[i].phase == ARRIVED)
		{
			//if arrived, return
			dist.lorries[i].daysUntilArrive = sellers[dist.lorries[i].sellerID].GetTravelTime();
			std::cout << fixed << setprecision(0) << "Truck  " << (i + 1) << " is returning from shop " << (dist.lorries[i].sellerID+1) << " and will arrive in " << (dist.lorries[i].daysUntilArrive) << " days\n";
			dist.lorries[i].phase = RETURNING;
		}
		else if (dist.lorries[i].phase == RETURNING)
		{
			//if returning, remove one from days until arrival
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout  << fixed << setprecision(0) << "Truck  "<< (i + 1) << " is returning from shop " << (dist.lorries[i].sellerID+1) << " and will arrive in "<< (dist.lorries[i].daysUntilArrive) << " days\n";
			}
			else
			{
				//if returned then set to idle
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << fixed << setprecision(0) << "Truck  "  << (i + 1)<<" has returned from shop " << to_string(dist.lorries[i].sellerID +1) << " and is waiting to be dispatched\n";
				dist.lorries[i].phase = IDLE;
			}
		}
	} 

}

//process of each day
void ProcessDay()
{
	//CheckLorries 
	CheckLorries();
	
	//each seller sells based on SPD(sales per day)
	for (int i = 0; i < numOfSellers; i++)
	{
		Sell(i);
	}
	
}



int main()
{


	srand(std::time(nullptr));

	//keep asking for input until correct input(case insensitve) is entered
	string input = "";
	cout << "Please enter either rule or r to use rule based or fuzzy or f to use fuzzy logic: ";
	while (input == "")
	{
		cin >> input;
		transform(input.begin(), input.end(), input.begin(),[](unsigned char c) { return std::tolower(c); });

		if (input == "f" || input == "fuzzy")
		{
			fuzzy = true;
		}
		else if (input == "r" || input == "rule")
		{
			fuzzy = false;
		}
		else
		{
			cout << "Error: Please enter either rule or r to use rule based or fuzzy or f to use fuzzy logic: ";
			input = "";
		}
	}

	//read in files
	ReadInFiles();
	//set up fuzzy logic requirements
	SetUpFuzzyLogic();

	//days to process(365 =one year)
	float daysToProcess = 365;
	
	std::cout << std::fixed;
	std::cout << std::setprecision(0);

	//processDays  
	for (int i = 1; i <= daysToProcess; i++)
	{
		std::cout << ("Day " + to_string(i) + ":\n\n") ; //output day
		ProcessDay();
		std::cout << ("\n");
	}

	//output sales missed for each shop
	float total = 0;
	for (int i = 0; i < numOfSellers; i++)
	{
		cout << fixed << setprecision(0) << ("\nSALES MISSED: ") <<  i << (" in shop ") <<(i+1) << ("\n");
		cout << fixed << setprecision(4) << ("\nSALES MISSED PER DAY: ") << (salesMissed[i] / daysToProcess) << ( " in shop ") << setprecision(4)<< (i + 1) << ( "\n");
		total += salesMissed[i];
		std::cout << ("\n");
	}

	//output total sales missed
	std::cout << ("------------------------------------------\n");

	cout << fixed << setprecision(0) << ("\nTOTAl SALES MISSED: ") <<  (total) << "\n";
	cout << fixed << setprecision(4) << ("\nTOTAL SALES MISSED PER DAY: " + to_string(total/daysToProcess) + "\n");

	//dont end straight away
	cin >> total;
   
	
}




