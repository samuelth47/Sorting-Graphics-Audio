#include <tsal.hpp>
#include <tsgl.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <cassert>
#include "sortUtilities.h"
using namespace std;
using namespace tsal;

void merge(vector<int>& data, int start, int mid, int end,
    ThreadSynth& synth, bool audio,
    tsgl::Rectangle** rectangles, tsgl::Canvas* can, bool graphics) {
  int start2 = mid + 1;

  // If the direct merge is already sorted
  if (data[mid] <= data[start2]) {
      return;
  }

  // Two pointers to maintain start
  // of both arrays to merge
  while (start <= mid && start2 <= end) {

      // If element 1 is in right place
      if (data[start] <= data[start2]) {
    if (audio) {
             MidiNote note = Util::scaleToNote<double>(data[start],
                         std::make_pair(0, MAX_VALUE),
             std::make_pair(C3, C7));
             synth.play(note, Timing::MICROSECOND, 50);
    }
    if (graphics) {
      rectangles[start]->setHeight(data[start]);
      if (!audio) can->sleepFor(GRAPHIC_WAIT);
    }
          ++start;
      } else {
    if (audio) {
             MidiNote note = Util::scaleToNote<double>(data[start2],
                         std::make_pair(0, MAX_VALUE),
             std::make_pair(C3, C7));
             synth.play(note, Timing::MICROSECOND, 50);
    }
    if (graphics) {
      rectangles[start2]->setHeight(data[start2]);
      if (!audio) can->sleepFor(GRAPHIC_WAIT);
    }
          int value = data[start2];
          int index = start2;
          // Shift all the elements between element 1
          // element 2, right by 1.
          while (index != start) {

              if (audio) {
                MidiNote note = Util::scaleToNote<double>(data[index],
                         std::make_pair(0, MAX_VALUE),
             std::make_pair(C3, C7));
                synth.play(note, Timing::MICROSECOND, 0);
              }
              if (graphics) {
                rectangles[index]->setHeight(data[index]);
                if (!audio) can->sleepFor(GRAPHIC_WAIT / 5000000);
              }
       
              data[index] = data[index - 1];
              --index;
          }
          data[start] = value;
          if (graphics) {
            rectangles[start]->setHeight(value);
          }

          // Update all the pointers
          ++start;
          ++mid;
          ++start2;
        }
    }
}

void mergeSort(vector<int>& data, int lo, int hi,
    ThreadSynth& synth, bool audio,
    tsgl::Rectangle** rectangles, tsgl::Canvas* can, bool graphics) {
  if(lo < hi) {
     int mid = lo + (hi - lo) / 2;
     if (audio) {
        MidiNote note = Util::scaleToNote<double>(data[mid],
                                     std::make_pair(0, data.size()),
                                     std::make_pair(C3, C7));
        synth.play(note, Timing::MICROSECOND, 50);
     }
     mergeSort(data, lo, mid, synth, audio, rectangles, can, graphics);
     mergeSort(data, mid+1, hi, synth, audio, rectangles, can, graphics);
     merge(data, lo, mid, hi, synth, audio, rectangles, can, graphics);
   } 
}

/** @example merge_sort.cpp
 * 
 * Merge sort is a sorting algorithm that can be done in parallel. As a result,
 * each thread involved in the algorithm is assigned an oscillator which plays
 * the pitch of the processed item plus a constant. Each thread has its own base
 * pitch so you can differentiate between them near the end of the sorting process.
 *
 * Parse the parameters\n
 * Create the mixer and oscillators\n
 * Start the merge sort algorithm
 * For each thread:
 * - Process an item in the merge step and set the oscillator pitch accordingly
 * - When complete with job, mute the oscillator 
 */
int main(int argc, char** argv) {
  bool audio, graphics;
  audio = false;
  graphics = false;
  int arg;

  while ((arg = getopt(argc, argv, "ag")) != -1) {
    switch (arg) {
      case 'a':
        audio = true;
        break;
      case 'g':
        graphics = true;
        break;
      default:
        break;
    }
  }

  // tsal setup
  Mixer mixer;
  ThreadSynth synth(&mixer);
  mixer.add(synth);
  synth.setVolume(0.5);
  synth.setEnvelopeActive(false);      
  
  // tsgl setup
  tsgl::Canvas* can;
  int w = SIZE;
  int h = MAX_VALUE * 1.2;
  if (graphics) {
    can = new tsgl::Canvas(0, 0, w, h, "Merge Sort", tsgl::BLACK);
    can->start();
  }

  // Generate data
  std::vector<int> data(SIZE);
  initialize(data);
  assert( !sorted(data) );

  // rectangles
  tsgl::Rectangle** rectangles = new tsgl::Rectangle*[SIZE];
  if (graphics) {
    float start = -can->getWindowWidth() * .45;
    float width = can->getWindowWidth() * .9 / SIZE;
    for (int i = 0; i < SIZE; i++) {
      rectangles[i] = new tsgl::Rectangle(roundf(start + i * width), 0, 0, width, data[i], 0, 0, 0, tsgl::RED);
      rectangles[i]->setIsOutlined(true);
      rectangles[i]->setOutlineColor(tsgl::RED);
      can->add(rectangles[i]);
    }
  }
  
  // sort data
  double startTime = omp_get_wtime();
  mergeSort(data, 0, SIZE-1, synth, audio, rectangles, can, graphics);
  double stopTime = omp_get_wtime();

  assert( sorted(data) );
  std::cout << "Time taken to sort " << SIZE << " items: "
            << stopTime - startTime << " seconds" << std::endl;

  synth.stop();
  if (graphics) can->wait();
}

