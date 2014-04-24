using namespace std;
#include <iostream>
#include "L1.h"

/*
  Flags:
  - noisy-development
  - half-decoder

 */

void L1::printDecoder(ofstream &stream)
{
    stream << endl << "Production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = productionRules.begin(); it != productionRules.end(); ++it)
    {
      stream <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	stream << *it2 << " ";
      stream << endl;
    }
    stream << "Gestation time: " << gestationTime << endl;
}

// Create a new decoder, for an `orphan' cell
void L1::newDecoder(config &args)
{
  productionRules.clear();

  if (not (args.modelConfig.count("type") and args.modelConfig["type"] == "ga"))
  {
    unsigned int s = rand()%(initProductionRuleSize-10) + 10;
    for(int i=0; i<s; ++i)
      productionRuleSpecification.push_back(rand()%alphabetSize);
  
    buildProductionRules(args, productionRuleSpecification.begin(), productionRuleSpecification.end(), productionRules);
  }
}


// Done at the start of each reproduction cycle
void L1::initializeDecoding(config &args)
{
  L1 *c = ((L1*)child);

  workingData.at(0).clear();
  workingData.at(1).clear();

  // Copy child's data to my working data
  // If this flag is set, introduce non-heritable variation at the start of development
  if (args.modelConfig.count("noisy-development"))
    workingData.at(workingDataFrame) = child->copyData(args);
  else
    copy(child->data.begin(), child->data.end(), back_inserter(workingData.at(workingDataFrame)));

}


void L1::buildProductionRules(config &args, vector<unsigned short>::iterator it, vector<unsigned short>::iterator decoderEnd, map<triple, vector<unsigned short> > &target)
{
  vector<unsigned short>::iterator decoderStart = it;
  // Build child's production rules from the stuff before the punctuation mark
  while (decoderEnd-it >= 3) // While there are enough symbols left to make the LHS of a production rule
  {
    triple LHS = {*it, *(it+1), *(it+2)};
    it += 3;
    vector<unsigned short>::iterator ruleEnd = decoderEnd;
    unsigned int punctuationSize = 0;
    // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
    for (vector<vector<unsigned short> >::iterator p = rulePunctuationMarks.begin(); p != rulePunctuationMarks.end(); ++p)
    {
      vector<unsigned short>::iterator ruleEnd2 = search(it, decoderEnd, p->begin(), p->end());
      if (ruleEnd2 - decoderStart < ruleEnd - decoderStart)
      {
	ruleEnd = ruleEnd2;
	punctuationSize = p->end() - p->begin();
      }
    }

    // TODO changed so that rules can only be so long
    if (ruleEnd - it > 10)
    {
      ruleEnd = it + 10;
      punctuationSize = 0;
    }

    vector<unsigned short> RHS;
    copy(it, ruleEnd, back_inserter(RHS));
    target[LHS] = RHS;
    it = ruleEnd + punctuationSize;
  }
}


// 1. Split the unpacked data into two parts, one of which specifies the production rules of the next generation, and the other of which specifies whatever else the environment requires
// 2. Build rules from the first part
void L1::interpret(config &args, vector<unsigned short> unpackedData)
{
  L1 *c = (L1*)child; 

  vector<unsigned short>::iterator decoderEnd = unpackedData.end();
  unsigned int punctuationSize = 0;
  // If loopy model, split the result into a specification of production rules and specification of the body
  if (type == LOOPY)
  {
    // Look for a part punctuation mark
    // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
    // todo I don't think this works if we've got more than one part punctuation mark. Look at the implementation of the rule punctuation searching below
    for (vector<vector<unsigned short> >::iterator p = partPunctuationMarks.begin(); p != partPunctuationMarks.end(); ++p)
    {
      vector<unsigned short>::iterator searchBegin;
      // If this flag is set, at least half of the unpacked data specifies a decoder, so we only look for the punctuation mark in the second half
      if (args.modelConfig.count("half-decoder"))
	searchBegin = unpackedData.begin() + unpackedData.size()/2;
      else
	searchBegin = unpackedData.begin();
      decoderEnd = search(searchBegin, unpackedData.end(), p->begin(), p->end());
      if (decoderEnd != unpackedData.end())
	punctuationSize = p->size();
    }
  }
  else // If unloopy model, the whole unpacked data specifies the body
    decoderEnd = unpackedData.begin();

  // Copy the stuff after the punctuation mark to the child's body specification
  c->bodySpecification.clear();
  copy(decoderEnd+punctuationSize, unpackedData.end(), back_inserter(c->bodySpecification));
  
  // If loopy, build the production rules using the stuff before the punctuation mark
  if (type == LOOPY)
    buildProductionRules(args, unpackedData.begin(), decoderEnd, c->productionRules);
  else if (type == UNLOOPY) // else copy our production rules specification to the child, mutated
  {
    copy(productionRuleSpecification.begin(), productionRuleSpecification.end(), back_inserter(c->productionRuleSpecification));

    // Mutate the production rules  TODO decision. How to mutate, min/max size
    if (rand()%2==0 and c->productionRuleSpecification.size() < initProductionRuleSize*10)
    {
      if (c->productionRuleSpecification.size() == 0)
	c->productionRuleSpecification.push_back(rand()%alphabetSize);
      else
	c->productionRuleSpecification.insert(c->productionRuleSpecification.begin()+rand()%(c->productionRuleSpecification.size()), rand()%alphabetSize);
    }
    // Delete with probability 0.5
    if (rand()%2==0 and c->productionRuleSpecification.size() > 0)
      c->productionRuleSpecification.erase(c->productionRuleSpecification.begin()+rand()%(c->productionRuleSpecification.size()));
    // Change with probability 0.5
    if (rand()%2==0 and c->productionRuleSpecification.size() > 0)
      c->productionRuleSpecification.at(rand()%(c->productionRuleSpecification.size())) = rand()%alphabetSize;

    c->buildProductionRules(args, c->productionRuleSpecification.begin(), c->productionRuleSpecification.end(), c->productionRules);
  }


  // If there hasn't been a mutation, check if the parent and child are identical
  if (not (child->identical == 2))
  {
    if (productionRules.size() == c->productionRules.size() and equal(productionRules.begin(), productionRules.end(), c->productionRules.begin()) )
      child->identical = 1;
  }


  // Debugging
  if(args.modelConfig.count("debug"))
  {
    cout << "Final working data:" << endl;
    for(vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;

    cout << id << " produced child " << c->id << endl;
  
    if (c->bodySpecification.size() > 0)
    {
      cout << "Child's body specification:" << endl;
      for(vector<unsigned short>::iterator it=c->bodySpecification.begin(); it!=c->bodySpecification.end(); ++it)
	cout << *it << " ";
      cout << endl;
    }

    cout << "Child's production rule specification:" << endl;
    if (type == LOOPY)
    {
      for(vector<unsigned short>::iterator it=unpackedData.begin(); it!=decoderEnd; ++it)
	cout << *it << " ";
    }
    else
    {
      for(vector<unsigned short>::iterator it=c->productionRuleSpecification.begin(); it!=c->productionRuleSpecification.end(); ++it)
	cout << *it << " ";      
    }
    cout << endl;

    cout << "Child's production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = c->productionRules.begin(); it != c->productionRules.end(); ++it)
    {
      cout <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	cout << *it2 << " ";
      cout << endl;
    }
  } // End of debugging

  // Check if the parent and child have identical production rules
  //bool identical = false;

}



// Iteratively apply the rewrite rules (decoder) to the data
void L1::decode(config &args)
{
  L1 *c = (L1*)child;

  // Debugging
  if(args.modelConfig.count("debug"))
  {
    cout << "I am " << id << " producing child " << c->id << endl;
    cout << "Data:" << endl;
    for(vector<unsigned short>::iterator it=data.begin(); it!=data.end(); ++it)
      cout << *it << " ";
    cout << endl << "Child's data:" << endl;
    for(vector<unsigned short>::iterator it=c->data.begin(); it!=c->data.end(); ++it)
      cout << *it << " ";
    cout << endl << "Production rule specification:" << endl;
    for(vector<unsigned short>::iterator it = productionRuleSpecification.begin(); it != productionRuleSpecification.end(); ++it)
      cout << *it << " ";
    cout << endl << "Production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = productionRules.begin(); it != productionRules.end(); ++it)
    {
      cout <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	cout << *it2 << " ";
      cout << endl;
    }
    cout << "Working data:" << endl;
    for(vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;
  } // End of debugging

  changedOnLastPass = false;
  workingData.at(1-workingDataFrame).clear();

  for (vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
  {

    // if we're over the working data limit, stop unpacking
    if (workingData.at(1-workingDataFrame).size() > maxWorkingDataSize)
    {
      workingData.at(1-workingDataFrame).erase(workingData.at(1-workingDataFrame).begin()+maxWorkingDataSize, workingData.at(1-workingDataFrame).end());
      changedOnLastPass = false;
      break;
    }

    // if there isn't a matching production rule, just copy the symbol
    // otherwise, copy the matched data

    // If the symbol doesn't have a left or right neighbour
    if (it-workingData.at(workingDataFrame).begin() == 0 or workingData.at(workingDataFrame).end()-it == 1)
    {
      workingData.at(1-workingDataFrame).push_back(*it);
    }
    else
    {
      triple t = {*(it-1),*it,*(it+1)};
      map<triple, vector<unsigned short> >::iterator productionRule = productionRules.end();

      // Treat a '3' on the left or right as a wildcard symbol
      if (args.modelConfig.count("wildcard"))
      {
	if (t.x == 3)
	{
	  triple t2 = t;
	  for (int i=0; i<alphabetSize; ++i)
	  {
	    t2.x = i;
	    productionRule = productionRules.find(t2);
	    if (productionRule != productionRules.end())
	      break;
	  }
	}
	if (t.z == 3 and productionRule == productionRules.end())
	{
	  triple t2 = t;
	  for (int i=0; i<alphabetSize; ++i)
	  {
	    t2.z = i;
	    productionRule = productionRules.find(t2);
	    if (productionRule != productionRules.end())
	      break;
	  }
	}
      }

      if (productionRule == productionRules.end())
	productionRule = productionRules.find(t);

      // if there is no such production rule
      if (productionRule == productionRules.end())
      {
	workingData.at(1-workingDataFrame).push_back(*it);
      }
      else
      {
	changedOnLastPass = true;
	// Insert from the production rule
	copy(productionRule->second.begin(), productionRule->second.end(), back_inserter(workingData.at(1-workingDataFrame)));	

	// Debugging
	if(args.modelConfig.count("debug"))
	{
	cout << "Replacing " << *it << " with ";
	for(vector<unsigned short>::iterator it2=productionRule->second.begin(); it2!=productionRule->second.end(); ++it2)
	  cout << *it2 << " ";
	cout << endl;
	} // End debugging
      }
    }
  }

  // Switch the frames
  workingDataFrame = 1-workingDataFrame;

  if(args.modelConfig.count("debug"))
  {
    cout << "Working data after one pass:" << endl;
    for(vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;
  }

  if (not changedOnLastPass) 
  {
    interpret(args, workingData.at(workingDataFrame));
    state=REPRODUCED;
  }

}

L1::L1(config &args) : BaseReplicator(args)
{
  type = LOOPY;
  if ( args.modelConfig.count("type") )
  {
    if (args.modelConfig["type"] == "ga")
      type = GA;
    else if (args.modelConfig["type"] == "unloopy")
      type = UNLOOPY;
  }

  initProductionRuleSize = initialDataSize * 5;
  maxWorkingDataSize  = maxDataSize * 10;

  workingDataFrame = 0;
  vector<unsigned short> workingData1;
  vector<unsigned short> workingData2;
  workingData.push_back(workingData1);
  workingData.push_back(workingData2);

  partPunctuationMarks.push_back({1,1});
  rulePunctuationMarks.push_back({0,0});
  rulePunctuationMarks.push_back({2,2});
  rulePunctuationMarks.push_back({3,3});
  rulePunctuationMarks.push_back({0,2});
  rulePunctuationMarks.push_back({0,3});
  rulePunctuationMarks.push_back({2,0});
  rulePunctuationMarks.push_back({2,3});
  rulePunctuationMarks.push_back({3,0});
  rulePunctuationMarks.push_back({3,2});
}

L1::~L1()
{
}
