using namespace std;
#include <iostream>
#include "L1.h"

void L1::printDecoder()
{
}

// Create a new decoder, for an `orphan' cell
void L1::newDecoder(config &args)
{
  // Initial decoder is empty, so implicitly rewrites everything to itself
  productionRules.clear();
}


// Done at the start of each reproduction cycle
void L1::initializeDecoding(config &args)
{
  L1 *c = ((L1*)child);

  workingData.at(0).clear();
  workingData.at(1).clear();

  // Copy child's data to my working data
  copy(child->data.begin(), child->data.end(), back_inserter(workingData.at(workingDataFrame)));

}



// 1. Split the unpacked data into two parts, one of which specifies the production rules of the next generation, and the other of which specifies whatever else the environment requires
// 2. Build rules from the first part
void L1::interpret(config &args, vector<unsigned short> unpackedData)
{
  L1 *c = (L1*)child; 

  /*cout << "Final working data:" << endl;
  for(vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
    cout << *it << " ";
  cout << endl << endl;

  cout << id << " produced child " << c->id << endl;*/

  // Look for a part punctuation mark
  vector<unsigned short>::iterator decoderEnd = unpackedData.end();
  unsigned int punctuationSize = 0;
  // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
  for (vector<vector<unsigned short> >::iterator p = partPunctuationMarks.begin(); p != partPunctuationMarks.end(); ++p)
  {
    decoderEnd = search(unpackedData.begin(), unpackedData.end(), p->begin(), p->end());
    if (decoderEnd != unpackedData.end())
      punctuationSize = p->size();
  }

  // Copy the stuff after the punctuation mark to the child's body specification
  c->bodySpecification.clear();
  copy(decoderEnd+punctuationSize, unpackedData.end(), back_inserter(c->bodySpecification));

  /*
  if (c->bodySpecification.size() > 0)
  {
    cout << "Child's body specification:" << endl;
    for(vector<unsigned short>::iterator it=c->bodySpecification.begin(); it!=c->bodySpecification.end(); ++it)
      cout << *it << " ";
    cout << endl;
  }
  */

  /*
  cout << "Child's production rule specification:" << endl;
  for(vector<unsigned short>::iterator it=unpackedData.begin(); it!=decoderEnd; ++it)
    cout << *it << " ";
    cout << endl;
  */

  // Build child's production rules from the stuff before the punctuation mark
  vector<unsigned short>::iterator it = unpackedData.begin();
  while (decoderEnd-it >= 3) // While there are enough symbols left to make the LHS of a production rule
  {
    triple LHS = {*it, *(it+1), *(it+2)};
    it += 3;
    vector<unsigned short>::iterator ruleEnd = decoderEnd;
    punctuationSize = 0;
    // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
    for (vector<vector<unsigned short> >::iterator p = rulePunctuationMarks.begin(); p != rulePunctuationMarks.end(); ++p)
    {
      ruleEnd = search(it, decoderEnd, p->begin(), p->end());
      if (ruleEnd != decoderEnd)
	punctuationSize = p->size();
    }
    vector<unsigned short> RHS;
    copy(it, ruleEnd, back_inserter(RHS));
    c->productionRules[LHS] = RHS;
    it = ruleEnd + punctuationSize;
  }

  /*cout << "Child's production rules:" << endl;
  for(map<triple, vector<unsigned short> >::iterator it = c->productionRules.begin(); it != c->productionRules.end(); ++it)
  {
    cout <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
    for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
      cout << *it2 << " ";
    cout << endl;
    }*/

}



// Iteratively apply the rewrite rules (decoder) to the data
void L1::decode(config &args)
{
  L1 *c = (L1*)child;

  /*cout << "I am " << id << " producing child " << c->id << endl;
  cout << "Data:" << endl;
  for(vector<unsigned short>::iterator it=data.begin(); it!=data.end(); ++it)
    cout << *it << " ";
  cout << endl << "Child's data:" << endl;
  for(vector<unsigned short>::iterator it=c->data.begin(); it!=c->data.end(); ++it)
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
    cout << endl << endl;*/

  changedOnLastPass = false;
  workingData.at(1-workingDataFrame).clear();

  for (vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
  {

    // if we're over the working data limit, stop unpacking
    if (workingData.at(1-workingDataFrame).size() > maxWorkingDataSize)
    {
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
      //cout << "Testing triplet " << t.x << " " << t.y << " " << t.z << endl;

      // if there is no such production rule
      map<triple, vector<unsigned short> >::iterator productionRule = productionRules.find(t);

      if (productionRule == productionRules.end())
      {
	workingData.at(1-workingDataFrame).push_back(*it);
      }
      else
      {
	changedOnLastPass = true;
	// Insert from the production rule
	copy(productionRule->second.begin(), productionRule->second.end(), back_inserter(workingData.at(1-workingDataFrame)));	
	/*cout << "Replacing " << *it << " with ";
	for(vector<unsigned short>::iterator it2=productionRule->second.begin(); it2!=productionRule->second.end(); ++it2)
	  cout << *it2 << " ";
	  cout << endl;*/
      }
    }
  }

  // Switch the frames
  workingDataFrame = 1-workingDataFrame;

  /*cout << "Working data after one pass:" << endl;
  for(vector<unsigned short>::iterator it=workingData.at(workingDataFrame).begin(); it!=workingData.at(workingDataFrame).end(); ++it)
    cout << *it << " ";
    cout << endl << endl;*/

  if (not changedOnLastPass) 
  {
    interpret(args, workingData.at(workingDataFrame));
    state=REPRODUCED;
  }

  //cout << endl;

}

L1::L1(config &args) : BaseReplicator(args)
{
  maxWorkingDataSize  = maxDataSize *100;

  workingDataFrame = 0;
  vector<unsigned short> workingData1;
  vector<unsigned short> workingData2;
  workingData.push_back(workingData1);
  workingData.push_back(workingData2);

  partPunctuationMarks.push_back({1,1});
  rulePunctuationMarks.push_back({0,0});
}

L1::~L1()
{
}
