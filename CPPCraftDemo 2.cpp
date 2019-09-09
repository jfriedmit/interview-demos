#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <ratio>

typedef unsigned int uint;
const char ColumnNameNumber = 6;


/**
    Represents a Record Object
*/
struct QBRecord
{
    uint column0; // unique id column
    std::string column1;
    long column2;
    std::string column3;
};

/**
Represents a Record Collections
*/
typedef std::vector<QBRecord> QBRecordCollection;

/**
    Return records that contains a string in the StringValue field
    records - the initial set of records to filter
    matchString - the string to search for
*/
QBRecordCollection QBFindMatchingRecords(const QBRecordCollection& records, const std::string& columnName, const std::string& matchString)
    {
    QBRecordCollection result;
	
	// Optimization notes:  Only checking the numerical character, instead of the entire field name,
	// reduced the runtime by appoximately one to one and a half milliseconds.
	// Only checking the column name once per execution reduced runtime by another one to one and a half ms.
	if (columnName[ColumnNameNumber] == '0') {
		std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec) {
			uint matchValue = std::stoul(matchString);
			return matchValue == rec.column0;
		});
		} else if (columnName[ColumnNameNumber] == '1') {
			// Switching to good "old-fashioned" iterators cuts run time by a factor of five.
			for (std::vector<QBRecord>::const_iterator it = records.begin(); it != records.end(); ++it) {
				if (it->column1 == matchString)
				{
					result.push_back(*it);
				}
			}
/*			std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec) {
				return rec.column1.find(matchString) != std::string::npos;
			}); */
		} else if (columnName[ColumnNameNumber] == '2') {
			for (std::vector<QBRecord>::const_iterator it = records.begin(); it != records.end(); ++it) {
				if (it->column2 == std::stol(matchString))
				{
					result.push_back(*it);
				}
			}
/*			std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec) {
				long matchValue = std::stol(matchString);
				return matchValue == rec.column2;
			}); */
		} else if (columnName[ColumnNameNumber] == '3') {
			std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec) {
				return rec.column3.find(matchString) != std::string::npos;
			});
        } 
		//else {
            //result = false;
        //}
    
    return result;
    }

/*
 * Removes the record with the given id, i.e. value in the "column0" field,
 * from the collection.
 */
void QBDeleteRecordByID(QBRecordCollection &records, uint id)
	{
	for (QBRecordCollection::iterator it = records.begin(); it != records.end(); ++it)
		if (it->column0 == id)
		{
			records.erase(it);
			break;  // id should be unique, so no need to continue
		}
	}

/**
    Utility to populate a record collection
    prefix - prefix for the string value for every record
    numRecords - number of records to populate in the collection
*/
QBRecordCollection populateDummyData(const std::string& prefix, uint numRecords)
    {
    QBRecordCollection data;
    data.reserve(numRecords);
    for (uint i = 0; i < numRecords; i++)
        {
        QBRecord rec = { i, prefix + std::to_string(i), i % 100, std::to_string(i) + prefix };
        data.emplace_back(rec);
        }
    return data;
    }

int main(int argc, _TCHAR* argv[])
{
    using namespace std::chrono;
    // populate a bunch of data
    auto data = populateDummyData("testdata", 1000);
    // Find a record that contains and measure the perf
    auto startTimer = steady_clock::now();
    auto filteredSet = QBFindMatchingRecords(data, "column1", "testdata500");
    auto filteredSet2 = QBFindMatchingRecords(data, "column2", "24");
    std::cout << "profiler: " << double((steady_clock::now() - startTimer).count()) * steady_clock::period::num / steady_clock::period::den << std::endl;

    // make sure that the function is correct
    assert(filteredSet.size() == 1);

	// Should be 10 records with column2 equal to 24, as there are 10 integers between 0 and 999 that end in 24.
	assert(filteredSet2.size() == 10);
	
	auto filteredSet3 = QBFindMatchingRecords(data, "column0", "314");
	assert(filteredSet3.size() == 1);
	QBDeleteRecordByID(data, 314u);
	filteredSet3 = QBFindMatchingRecords(data, "column0", "314");
	assert(filteredSet3.size() == 0);
	return 0;
}

