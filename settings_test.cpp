#include "get_settings.h"
#include <bits/stdc++.h>

using namespace std;

int main()
{
	cout << get_settings::get_variable("/home/patrick/Documents/UCL/BLabs/lib/settings", "NEUTRAL_PH_LEVEL") << endl;
	cout << get_settings::store_variable("/home/patrick/Documents/UCL/BLabs/lib/settings", "ACID_PH_LEVEL", 10) << endl;
}