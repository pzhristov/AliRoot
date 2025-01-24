//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUExtractPbPbCollision.h
/// \author David Rohr

static void GPUExtractPbPbCollision(GPUParam& param, GPUTrackingInOutPointers& ioPtrs)
{
  std::vector<uint32_t> counts(param.continuousMaxTimeBin + 1);
  std::vector<uint32_t> sums(param.continuousMaxTimeBin + 1);
  std::vector<uint32_t> countsTracks(param.continuousMaxTimeBin + 1);
  std::vector<uint32_t> sumsTracks(param.continuousMaxTimeBin + 1);
  std::vector<bool> mask(param.continuousMaxTimeBin + 1);
  const int32_t driftlength = 520;
  const bool checkAfterGlow = true;
  const int32_t afterGlowLength = checkAfterGlow ? 8000 : 0;
  for (uint32_t i = 0; i < ioPtrs.clustersNative->nClustersTotal; i++) {
    int32_t time = ioPtrs.clustersNative->clustersLinear[i].getTime();
    if (time < 0 || time > param.continuousMaxTimeBin) {
      fprintf(stderr, "Invalid time %d > %d\n", time, param.continuousMaxTimeBin);
      throw std::runtime_error("Invalid Time");
    }
    counts[time]++;
  }
  for (uint32_t i = 0; i < ioPtrs.nMergedTracks; i++) {
    if (ioPtrs.mergedTracks[i].NClusters() < 40) {
      continue;
    }
    int32_t time = ioPtrs.mergedTracks[i].GetParam().GetTZOffset();
    if (time < 0 || time > param.continuousMaxTimeBin) {
      continue;
    }
    countsTracks[time]++;
  }
  int32_t first = 0, last = 0;
  for (int32_t i = driftlength; i < param.continuousMaxTimeBin; i++) {
    if (counts[i]) {
      first = i;
      break;
    }
  }
  for (int32_t i = param.continuousMaxTimeBin + 1 - driftlength; i > 0; i--) {
    if (counts[i - 1]) {
      last = i;
      break;
    }
  }
  uint32_t count = 0;
  uint32_t countTracks = 0;
  uint32_t min = 1e9;
  uint64_t avg = 0;
  for (int32_t i = first; i < last; i++) {
    count += counts[i];
    countTracks += countsTracks[i];
    if (i - first >= driftlength) {
      sums[i - driftlength] = count;
      sumsTracks[i - driftlength] = countTracks;
      if (count < min) {
        min = count;
      }
      avg += count;
      count -= counts[i - driftlength];
      countTracks -= countsTracks[i - driftlength];
    }
  }
  avg /= (last - first - driftlength);
  printf("BASELINE Min %d Avg %d\n", min, (int32_t)avg);
  /*for (int32_t i = first; i < last - driftlength; i++) {
    printf("STAT %d: %u %u (trks %u)\n", i, sums[i], counts[i], sumsTracks[i]);
  }*/
  bool found = false;
  do {
    found = false;
    uint32_t max = 0, maxpos = 0;
    for (int32_t i = first; i < last - driftlength - afterGlowLength; i++) {
      if (sums[i] > 10 * min && sums[i] > avg && sumsTracks[i] > 3) {
        bool noColInAfterGlow = true;
        if (checkAfterGlow) {
          for (int32_t ii = i + driftlength; ii < i + driftlength + afterGlowLength; ii++) {
            if (sums[ii] > 10 * min && sums[ii] > avg && sumsTracks[ii] > 3) {
              noColInAfterGlow = false;
            }
          }
        }
        if (noColInAfterGlow && sums[i] > max) {
          max = sums[i];
          maxpos = i;
          found = true;
        }
      }
    }
    if (found) {
      uint32_t glow = 0;
      uint32_t glowcount = 0;
      if (checkAfterGlow) {
        int32_t glowstart = maxpos + driftlength;
        int32_t glowend = std::min<int32_t>(last, maxpos + driftlength + afterGlowLength);
        for (int32_t i = glowstart; i < glowend; i++) {
          glowcount++;
          glow += counts[i];
        }
        // printf("AFTERGLOW RANGE %d %d\n", glowstart, glowend);
      }
      printf("MAX %d: %u (Tracks %u) Glow %d (%d)\n", maxpos, max, sumsTracks[maxpos], glow, glowcount);
      for (int32_t i = std::max<int32_t>(first, maxpos - driftlength); i < std::min<int32_t>(last, maxpos + driftlength + afterGlowLength); i++) {
        sums[i] = 0;
        mask[i] = true;
      }
    }
  } while (found && !checkAfterGlow);
  uint32_t noise = 0;
  uint32_t noisecount = 0;
  for (int32_t i = first; i < last; i++) {
    if (!mask[i]) {
      noise += counts[i];
      noisecount++;
    }
  }
  printf("AVERAGE NOISE: %d\n", noise / noisecount);
}
