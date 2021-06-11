/* insertion_sort.cpp
 *
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <tsal.hpp>
#include <tsgl.h>
#include <omp.h>
#include <unistd.h>
#include "sortUtilities.h"
using namespace tsal;


void insertionSort(vector<int>& data, ThreadSynth& synth, tsgl::Rectangle** rectangles, tsgl::Canvas* can, bool audio, bool graphics) {
  for (int i = 1; i <= SIZE; ++i) {
    int insertValue = data[i];
    int j = i;
    if (audio) {
       MidiNote note = Util::scaleToNote(data[i],
                                            std::make_pair(0, MAX_VALUE),
                                            std::make_pair(C3, C7));
       synth.play(note, Timing::MICROSECOND, 50);
    }
    
    if (graphics) {
      if (!audio) can->sleepFor(GRAPHIC_WAIT);
      rectangles[i]->setHeight(data[i]);
    }
    while (j > 0 && data[j-1] > insertValue) {


      data[j] = data[j-1];
      --j;
      if (audio) {
         MidiNote note = Util::scaleToNote(data[j],
                                            std::make_pair(0, MAX_VALUE),
                                            std::make_pair(C3, C7));
         synth.play(note, Timing::MICROSECOND, 50);
      }
      if (graphics) {
        if (!audio) can->sleepFor(GRAPHIC_WAIT);
        rectangles[j]->setHeight(data[j-1]);
      }
    }
    data[j] = insertValue;
    if (audio) {
      MidiNote note = Util::scaleToNote(insertValue, std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
      synth.play(note, Timing::MICROSECOND, 50);
    }
    if (graphics) {
      if (!audio) can->sleepFor(GRAPHIC_WAIT);
      rectangles[j]->setHeight(insertValue);
    }
  }
}

int main(int argc, char** argv) {
  bool graphics = false;
  bool audio = false;
  int opt;

  while ((opt = getopt(argc, argv, "ag")) != -1) {
    switch (opt) {
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

  // Generate data
  vector<int> data(SIZE);
  initialize(data);
  assert( !sorted(data) );

  //tsgl setup
  tsgl::Canvas* can;
  int w = SIZE;
  int h = MAX_VALUE * 1.2;
  if (graphics) {
    can = new tsgl::Canvas(0, 0, w, h, "Insertion Sort", tsgl::BLACK);
    can->start();
  }

  // tsal setup
  Mixer mixer;
  ThreadSynth synth = ThreadSynth(&mixer);
  mixer.add(synth);
  synth.setEnvelopeActive(false);

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

  // Sort the data
  double startTime = omp_get_wtime();
  insertionSort(data, synth, rectangles, can, audio, graphics);
  double stopTime = omp_get_wtime();

  assert( sorted(data) );
  std::cout << "Time taken to sort " << SIZE << " items: "
            << stopTime - startTime << " seconds" << std::endl;
  synth.stop();
  can->wait();

} 
