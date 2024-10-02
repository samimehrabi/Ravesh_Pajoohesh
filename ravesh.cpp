#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random>
#include <ctime>
#include <set>

using namespace std;

// Function to find the highest priority topic for a group from the remaining topics
int find_highest_priority_topic(const vector<int>& priorities, const set<int>& remaining_topics) {
    int max_priority = -1;
    int best_topic = -1;

    for (int topic : remaining_topics) {
        if (priorities[topic] > max_priority) {
            max_priority = priorities[topic];
            best_topic = topic;
        }
    }

    return best_topic;
}

// Function to check if the topic is a repeatable topic (can be assigned to more than one group)
bool is_repeatable_topic(int topic) {
    // Topics 2, 10, 13, 14 are repeatable (can be assigned to more than one group)
    return (topic == 1 || topic == 9 || topic == 12 || topic == 13 || topic == 14);  
}

int main() {
    int num_groups = 12, num_topics = 14;  // Updated to 12 groups

    // List of topics
    vector<string> topics;
    for (int i = 1; i <= num_topics; ++i) {
        topics.push_back("Topic " + to_string(i));
    }

    // List of groups
    vector<string> groups;
    for (int i = 1; i <= num_groups; ++i) {
        groups.push_back("Group " + to_string(i));
    }

    // Get the priorities of each group from the user
    map<string, vector<int>> priorities;
    for (int i = 0; i < num_groups; ++i) {
        cout << "Enter the priorities for " << groups[i] << " (numbers between 1 and " << num_topics << "):\n";
        vector<int> priority_list(num_topics);
        for (int j = 0; j < num_topics; ++j) {
            cout << "Priority for " << topics[j] << ": ";
            cin >> priority_list[j];
        }
        priorities[groups[i]] = priority_list;
    }

    // Set of remaining topics to be allocated
    set<int> remaining_topics;
    for (int i = 0; i < num_topics; ++i) {
        remaining_topics.insert(i);
    }

    // Set of remaining groups to receive topics
    set<string> remaining_groups(groups.begin(), groups.end());

    // Map to store the final allocation of topics to groups
    map<string, string> allocation;
    vector<string> final_allocation_order;  // To track the order of assignments

    // While there are still topics and groups to be allocated
    while (!remaining_topics.empty() && !remaining_groups.empty()) {
        bool topic_assigned = false;

        // Check if any group has a unique highest priority topic
        for (const auto& group : remaining_groups) {
            vector<int> group_priorities = priorities[group];

            // Find the topic with the highest priority for this group
            int best_topic = find_highest_priority_topic(group_priorities, remaining_topics);

            // Check if this topic is only wanted by this group as the highest priority
            int max_priority_for_group = group_priorities[best_topic];
            bool is_unique = true;

            for (const auto& other_group : remaining_groups) {
                if (other_group != group) {
                    vector<int> other_priorities = priorities[other_group];
                    if (other_priorities[best_topic] >= max_priority_for_group) {
                        is_unique = false;
                        break;
                    }
                }
            }

            // If this topic is uniquely the highest priority for this group, assign it
            if (is_unique) {
                allocation[group] = topics[best_topic];
                final_allocation_order.push_back(group);  // Track the order of selection
                remaining_groups.erase(group);

                // If the topic is not repeatable, remove it from remaining topics
                if (!is_repeatable_topic(best_topic)) {
                    remaining_topics.erase(best_topic);
                }

                cout << group << " has been directly assigned " << topics[best_topic] << " (unique highest priority)." << endl;
                topic_assigned = true;
                break;
            }
        }

        // If no topic was uniquely assigned, proceed to lottery based on the highest priority
        if (!topic_assigned) {
            // Find all groups with the highest priority for the remaining topics
            map<int, vector<string>> topic_to_groups;
            for (const auto& group : remaining_groups) {
                vector<int> group_priorities = priorities[group];
                int best_topic = find_highest_priority_topic(group_priorities, remaining_topics);
                topic_to_groups[best_topic].push_back(group);
            }

            // Sort topics by the number of interested groups (to manage ties)
            vector<pair<int, int>> sorted_topics;
            for (const auto& topic : topic_to_groups) {
                sorted_topics.push_back({topic.first, (int)topic.second.size()});
            }

            // Sort topics by number of interested groups (descending)
            sort(sorted_topics.begin(), sorted_topics.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;  // Sort by the number of interested groups
            });

            // Choose the topic with the highest number of interested groups
            int chosen_topic = sorted_topics[0].first;
            vector<string> interested_groups = topic_to_groups[chosen_topic];

            // Print the groups participating in the lottery
            cout << "\nLottery will be held for " << topics[chosen_topic] << " among the following groups:\n";
            for (const auto& group : interested_groups) {
                cout << group << " ";
            }
            cout << endl;

            // Check if only one group is interested in the selected topic
            if (interested_groups.size() == 1) {
                allocation[interested_groups[0]] = topics[chosen_topic];
                final_allocation_order.push_back(interested_groups[0]);  // Track the order of selection
                remaining_groups.erase(interested_groups[0]);

                // If the topic is not repeatable, remove it from remaining topics
                if (!is_repeatable_topic(chosen_topic)) {
                    remaining_topics.erase(chosen_topic);
                }

                cout << interested_groups[0] << " has been assigned " << topics[chosen_topic] << " (only group interested).\n";
            } else {
                // Randomly select a group from the interested groups
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dist(0, interested_groups.size() - 1);
                int winner_index = dist(gen);
                string winner_group = interested_groups[winner_index];

                // Assign the topic to the randomly chosen group
                allocation[winner_group] = topics[chosen_topic];
                final_allocation_order.push_back(winner_group);  // Track the order of selection
                remaining_groups.erase(winner_group);

                // If the topic is not repeatable, remove it from remaining topics
                if (!is_repeatable_topic(chosen_topic)) {
                    remaining_topics.erase(chosen_topic);
                }

                cout << "Lottery result: " << winner_group << " has been assigned " << topics[chosen_topic] << ".\n";
            }
        }

        // Check if there are topics left
        if (!remaining_topics.empty()) {
            // Ask if the user wants to continue to the next lottery
            char continue_lottery;
            cout << "Do you want to continue to the next lottery? (y/n): ";
            cin >> continue_lottery;

            if (continue_lottery == 'n' || continue_lottery == 'N') {
                break;
            }
        }
    }

    // Display the final topic assignment in the order they were assigned
    cout << "\nFinal allocation of topics to groups (in order of selection):\n";
    for (const auto& group : final_allocation_order) {
        cout << group << ": " << allocation[group] << endl;
    }

    // Wait for the user to press Enter before closing the program
    cout << "\nPress Enter to exit...";
    cin.ignore();  // To ignore the leftover newline
    cin.get();     // To wait for Enter key press

    return 0;
}
