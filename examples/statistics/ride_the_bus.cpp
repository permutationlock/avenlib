/*
 * ride_the_bus.cpp
 * Author: Aven Bross
 * Date: February 17, 2015
 * 
 * Simulating "Riding the bus" to attempt to find the average
 * number of drinks the player will have to take.
 */
 
#include <cmath>
#include <random>
#include <iostream>
#include <vector>

#include "../../statistics/stats.h"

using std::vector;
using std::cout;

/*
 * Deck class:
 * Uses mersenne twister to allow good random card draws.
 * Cards are drawn without replacement.
 */
class Deck {
public:
    // Constructor fills deck with 4 of each card (52 card deck)
    Deck(){
        for(int i=0; i<13; i++){
            for(int j=0; j<4; j++){
                cards.push_back(i);
            }
        }
    }
    
    // Draw a random card from the deck
    int draw(){
        // cout<<"Got to func\n";
        std::random_device rd;	//Create random device
        // cout<<"Made rd\n";
        std::mt19937 el(rd());	//Use 32 bit mersenne twister
        // cout<<"Made el\n";
        std::uniform_int_distribution<int> cdist(0,cards.size()-1);
        // cout<<"Made dist\n";
        
        int draw = cdist(el);
        // cout<<draw<<"\n";
        int card = cards[draw];
        cards.erase(cards.begin()+draw);
        
        return card;
    }

private:
	vector<int> cards;
};

// How many rounds are we playing?
const int rounds = 100000;

int main(){
    // Records drinks taken
    vector<int> records;
    
    // Play several rounds of the game
    for(int i=0; i<rounds; i++){
        bool win = false;
        int drinks = 0;
        
        // Drink until you win
        while(!win){
            // cout << "Making Decks\n";
            Deck deck;
            vector<int> drawn;
            
            // cout << "Drawing first card\n";
            int card = deck.draw();
            
            win=true;
            
            // Must guess right 6 times to win
            for(int t=0; t<6; t++){
                
                // Count cards above and below
                int above = (12-card)*4;
                int below = card*4;
                
                // Taking account past draws
                for(auto old_card : drawn){
                    if(old_card > card)
                        above--;
                    else if(old_card < card)
                        below--;
                }
                
                // Saving current card
                drawn.push_back(card);
                int pcard = card;
                
                // Draw new card
                card = deck.draw();
                
                // Checking guess
                if(above>below){
                    // We guess above, are we wrong?
                    if(card<pcard){
                        drinks += 1;
                        win = false;
                        break;
                    }
                }
                else{
                    // We guess below, are we wrong?
                    if(card>pcard){
                        drinks += 1;
                        win = false;
                        break;
                    }
                }
            }
        }
        
        records.push_back(drinks);
    }
    
    oneVarStats(records);
}
