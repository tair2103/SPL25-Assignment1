#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService(): active_deck(0){
    decks[0] = nullptr;
    decks[1] = nullptr;

    auto_sync = false;
    bpm_tolerance = 0;
    std::cerr <<"[MixingEngineService] Initialized with 2 empty decks";
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cerr <<"[MixingEngineService] Cleaning updecks...";

    for(int i = 0; i < 2; ++i){
        if(decks[i] != nullptr){
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cerr << "\n=== Loading Track to Deck ===\n";
    PointerWrapper<AudioTrack> curr = track.clone();

    if (!curr) {
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone\n";
        return -1;
    }

    int target = 1 - active_deck;
    std::cerr << "[Deck Switch] Target deck: " << target << "\n";

    if(decks[target] != nullptr){
        delete decks[target];
        decks[target] = nullptr;
    }

    curr->load();
    curr->analyze_beatgrid();

    if (decks[active_deck] != nullptr && auto_sync){
        if(!can_mix_tracks(curr)){
            sync_bpm(curr);
        }
    }

    decks[target] = curr.release();
    std::cerr << "[Load Complete] " << decks[target]->get_title() << " is now loaded on deck "<< target << "\n";

    size_t previous_active_deck = active_deck; 
    active_deck = target;
    if (decks[previous_active_deck] != nullptr) {
        std::cerr << "[Unload] Unloading previous active deck: " << previous_active_deck << "\n";
        delete decks[previous_active_deck];
        decks[previous_active_deck] = nullptr;
    }
    std::cerr << "[Active Deck] Switched to deck  " << target << "\n";
    return target;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck] != nullptr){
        if (track) {
            int active_deck_bpm = decks[active_deck]->get_bpm();
            int new_track_bpm = track->get_bpm();
            if(std::abs(active_deck_bpm - new_track_bpm) <= bpm_tolerance){
                return true;
            }
        }
    }
    return false;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck] != nullptr && track){
        int active_deck_bpm = decks[active_deck]->get_bpm();
        int new_track_bpm = track->get_bpm();

        int averaged_bpm = (active_deck_bpm + new_track_bpm) / 2;
        track->set_bpm(averaged_bpm);
        std::cerr << "[Sync BPM] Syncing BPM from " << new_track_bpm << " to " << averaged_bpm << "\n";
    }
}
