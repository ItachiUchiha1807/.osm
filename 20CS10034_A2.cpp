/*
In this program there has been a menu provided for the functions which are to be integrated

Menu Structure:

1> Print the number of ways and number of nodes
2> Search for name of node via a substring
3> Find k nearest nodes to a node
4> Find the shortest path between two nodes
5> Exit the application

*/



#include "rapidxml.hpp"
#include <bits/stdc++.h>
# define INF 1e25
using namespace std;
using namespace rapidxml;

//structure to store id, name and type of element
struct name_type{
    long long int id;
    string name;
    string type;
};

class xml{ public:
    // Variable declaration
    // cnt_way, cnt_node store number of ways and node respectively
    long long int cnt_node = 0, cnt_way=0;

    //node_map which stores id, <lattitude, longitude> for all nodes
    unordered_map<long long int, pair<long double, long double>> node_map;

    //way_map stores id, vector of node ids on the way for all ways
    unordered_map<long long int, vector<long long int>> way_map;

    //name_element stores vector of structure name_type
    vector<struct name_type> name_element;


    xml(){
        xml_document<> doc;
        xml_node<> * root_node = NULL;
        ifstream theFile ("map.osm");
        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);

        //Setting the root node as osm
        root_node = doc.first_node("osm");

        //Iterating through the file to extract data from it
        for(xml_node<> * node = root_node->first_node(); node; node = node->next_sibling()){

            //If attribute lat(i.e. lattitude) is not null implies that it must be a node
            if(node->first_attribute("lat")!= NULL){
                long long int id = stoll(node->first_attribute("id")->value(),nullptr, 10);
                long double lat = stold(node->first_attribute("lat")->value(),nullptr);
                long double lon = stold(node->first_attribute("lon")->value(),nullptr);

                //Iterating through tag element in node if present, as it would store the name of the node
                for(xml_node<> *tags = node->first_node("tag"); tags; tags = tags->next_sibling()){
                    if(tags->first_attribute("k")!= NULL){
                        //If attribute k inside tag element is name, then it stores the name of the node
                        if(std::string(tags->first_attribute("k")->value()) == "name"){
                            struct name_type temp;
                            temp.id = id, temp.type = "node";
                            temp.name = std::string(tags->first_attribute("v")->value());
                            name_element.push_back(temp);
                            break;
                        }
                    }
                }
                node_map.insert({id, make_pair(lat, lon)});
                cnt_node++;
            }

            // If an element contains an element named nd then it must be a way
            else if(node->first_node("nd")!=NULL){
                long long int id = stoll(node->first_attribute("id")->value(),nullptr, 10);

                //Iterating through tag element in way if present, as it would store the name of the way
                for(xml_node<> *tags = node->first_node("tag"); tags; tags = tags->next_sibling()){
                    if(tags->first_attribute("k")!= NULL){
                        if(std::string(tags->first_attribute("k")->value()) == "name"){
                            struct name_type temp;
                            temp.id = id, temp.type = "way";
                            temp.name = std::string(tags->first_attribute("v")->value());
                            name_element.push_back(temp);
                            break;
                        }
                    }
                }

                //Vector to store the id's of all nodes present in a way
                vector <long long int> temp;
                for(xml_node<> *tags = node->first_node("nd"); tags; tags = tags->next_sibling()){
                    if(tags->first_attribute("ref")!= NULL){
                        long long int ref_id = stoll(tags->first_attribute("ref")->value(),nullptr, 10);
                        temp.push_back(ref_id);
                    }
                }
                way_map.insert({id,temp});
                cnt_way++;
            }
        }
    }

    //Function which prints the number of nodes and ways present in the file
    void cnt(){
        cout <<"\nThe number of nodes present are: " << cnt_node;
        cout <<"\nThe number of ways present are: "<<cnt_way;
    }

    //For part-1 matching input substring with names of the elements
    void string_matcher(){
        string substring;
        int input = 0;
        while(input!= 1){
            int pos = 0;
            cout << "\nPlease give substring of name: ";
            cin >> substring;

            //case_sens variable to check whether used wants case sensitive/ case insensitive checking
            //max_tries stands for maximum number of tries with wrong input (set to 5)
            int case_sens = -1, max_tries = 0;
            while(case_sens != 1 && case_sens != 0){
                cout << "\nPlease input 1 to get case insensitive search and 0 for case sensitive search: ";
                cin >> case_sens;
                if(max_tries == 5){
                    cout <<"Entered input wrong too many times";
                    return;
                }
                if(case_sens != 1 && case_sens != 0){
                    cout << "Please enter a valid input either 0 or 1";
                    max_tries++;
                }
            }

            //Flag variable becomes 1 if we find at least one match, if not it is used to print that no names were found
            int flag = 0;
            for(long long int i=0; i<name_element.size(); i++){
                string name = name_element[i].name;
                if(case_sens == 1){
                    transform(name.begin(), name.end(), name.begin(), ::tolower);
                    transform(substring.begin(), substring.end(), substring.begin(), ::tolower);
                }
                if(name.find(substring,pos) != string::npos){
                    flag = 1;
                    cout <<"\n\nName: "<<name_element[i].name;
                    cout <<"\nElement type: "<<name_element[i].type;
                    cout <<"\nID: "<< name_element[i].id;
                    if(name_element[i].type == "node"){
                        auto temp = node_map.find(name_element[i].id);
                        cout <<"\nLattitude: "<<(temp->second).first<<", Longitude: "<<(temp->second).second;
                    }
                }
            }
            if(flag == 0){
                cout << "\nThere are no nodes with '"<< substring << "' as substring of its name\n";
            }
            cout <<"\nPress 1 to quit searching for nodes by name, press any other number to continue: ";
            cin >> input;
            cout <<"\n";
        }
    }

    //Part 2 of the question (k nearest nodes)
    void k_nearest(){
        long long int k, node_id;
        int input_2 = 0;
        while(input_2!= 1){
            cout<<"Please give the value of k: ";
            cin >> k;
            int flag = 0, max_tries = 0;
            // To check whether node id entered is correct or not
            while(flag != 1){
                cout <<"Please enter the node id: ";
                cin >> node_id;
                if(node_map.find(node_id) == node_map.end()){
                    cout << "There is no node with the id "<<node_id <<". Please enter a valid node id\n";
                    max_tries++;
                }
                else{
                    flag =1;
                }
                if(max_tries == 5){
                    cout <<"Entered input wrong too many times";
                    return;
                }
            }
            vector <pair<int, long double>> dist;
            auto node = node_map.find(node_id);
            long double lat_node = (node->second).first, long_node = (node->second).second;
            for(auto i : node_map){
                if(i.first == node_id){
                    continue;
                }
                else{
                    pair <long double, long double> temp_coord = i.second;
                    dist.push_back({haversine(lat_node, long_node, temp_coord.first, temp_coord.second), i.first});
                }
            }
            nth_element(dist.begin(), dist.begin() + k, dist.end());
            sort(dist.begin(), dist.begin() + k);
            for(long long int i=0; i<k; i++){
                cout <<"\nID of node" << i+1 << ": "<<(long long int)dist[i].second <<"\n";
                cout <<"Distance between given node and this node: "<<(long long int)dist[i].first<<"\n";
                auto temp_node = node_map.find((long long int)dist[i].second);
                cout << "Lattitude of node "<< i+1<< ": "<< (temp_node -> second).first<< ", Longitude of node "<< i+1<<": "<< (temp_node->second).first<< "\n";
            }
            cout <<"\nPress 1 to quit , press any other number to continue: ";
            cin >> input_2;
            cout <<"\n";
        }
    }

    //To find the shortest path between two given nodes
    void dijkstra(){
        //Storing weights
        unordered_map<long long int, vector<pair<long long int, long double>>> adj;
        for(auto i : way_map){
            vector<long long int> temp = i.second;
            for(long long int j=0; j<(temp.size()-1); j++){
                long double dist = haversine(node_map[temp[j]].first, node_map[temp[j]].second, node_map[temp[j+1]].first, node_map[temp[j+1]].second);
                adj[temp[j]].push_back({temp[j+1], dist});
                adj[temp[j+1]].push_back({temp[j], dist});
            }
        }
        unordered_map <long long int, long double> distances;
        int input_3 = 0;
        while(input_3 != 1){
            set<pair<long double,long long int>> sets;
            int max_tries = 0;
            long long int source_id;
            while(true){
                cout << "\nPlease input the source id: ";
                cin >> source_id;
                if(node_map.find(source_id) == node_map.end()){
                    cout <<"There is no node with id "<<source_id<<"\nPlease enter a valid id\n";
                    max_tries++;
                    if(max_tries == 4){
                        cout <<"\nEntered wrong input too many times";
                        return;
                    }
                }
                else{
                    break;
                }
            }
            long long int dest_id;
            int max_tries_2 = 0;
            while(true){
                cout << "\nPlease input the destination id: ";
                cin >> dest_id;
                if(node_map.find(dest_id) == node_map.end()){
                    cout <<"There is no node with id "<<dest_id<<"\nPlease enter a valid id\n";
                    max_tries_2++;
                    if(max_tries_2 == 4){
                        cout <<"\nEntered wrong input too many times";
                        return;
                    }
                }
                else{
                    break;
                }
            }
            for(auto i: node_map){
                distances[i.first] = INF;
            }
            distances[source_id] = 0;
            sets.insert({0, source_id});

            //The main dijkstra algorithm
            while(!sets.empty()){
                pair<long double, long long int> temp = *(sets.begin());
                sets.erase(sets.begin());
                long long int u = temp.second;
                vector<pair<long long int, long double>> adj_vect = (adj.find(u))->second;
                for(long long int i = 0; i<adj_vect.size(); i++){
                    long long int v = adj_vect[i].first;
                    long double weight = adj_vect[i].second;
                    if(distances[v] > distances[u] + weight){
                        if(distances[v] != INF){
                            sets.erase(sets.find({distances[v],v}));
                        }
                        distances[v] = distances[u] + weight;
                        sets.insert({distances[v], v});
                    }
                }
            }

            //Prints the shortest path if distance between nodes isnt infinite, if not it prints cant find any path
            if(distances[dest_id] != INF){
                cout <<"\nThe shortest distance from "<< source_id <<" to "<<dest_id<<" is: "<<distances[dest_id] <<" meters";
            }
            else{
                cout <<"\nThere is no path between "<<source_id << " and "<< dest_id;
            }
            cout <<"\nPress 1 to quit, press any other number to continue: ";
            cin >> input_3;
            cout <<"\n";
        }
    }

    //Function to calculate the crow fly distance in metres
    long double haversine(long double lat1, long double lon1,long double lat2, long double lon2){
        long double dLat = (lat2 - lat1) *M_PI / (long double)180.0000;
        long double dLon = (lon2 - lon1) *M_PI / (long double)180.00000;
        lat1 = (lat1) * M_PI / (long double)180.00000;
        lat2 = (lat2) * M_PI / (long double)180.00000;
        long double a = pow(sin(dLat / 2), 2) +pow(sin(dLon / 2), 2) *cos(lat1) * cos(lat2);
        long double rad = 6371000;
        long double c = 2 * asin(sqrt(a));
        return rad * c;
    }
};




int main(void){

    // Made a menu like structure to make it user friendly to check various functions
    int input = -1;
    cout << "Starting parsing of the file :-\n";
    xml osm;
    while(input != 5){
        cout <<"\n\nThe options to use are: \n1> Print the number of ways and number of nodes\n2> Search for name of node via a substring "<<
                                                           "\n3> Find k nearest nodes to a node\n4> Find the shortest path between two nodes"<<
                                                           "\n5> Exit the application" <<"\nEnter option number: ";
        //Switch case statement based on input
        cin >> input;
        switch (input){
            case 1:
                osm.cnt();
                break;
            case 2:
                osm.string_matcher();
                break;
            case 3:
                osm.k_nearest();
                break;
            case 4:
                osm.dijkstra();
                break;
            case 5:
                cout <<"\nThank you\n";
                break;
            default:
                cout <<"Please enter a valid input number between 1-5\n";
        }
    }
}
