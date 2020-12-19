#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "Extent.h"

namespace Geometry
{
    struct Bvh
    {
        std::shared_ptr<Bvh> LeftNode;
        std::shared_ptr<Bvh> RightNode;

        Geometry::Extent Boundary;
        std::vector<PolygonExtent> Extents;
    };

    class BvhHelper
    {
    public:
        static inline void SortExtentsByEveryAxis(std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMin,
                                                  std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMax);

        static inline void ShrinkVolume(const int& i, const int& e, const int& n_extents,
                                        const std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMin,
                                        const std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMax,
                                        int* min_idx, int* max_idx,
                                        float* right_min, float* right_max,
                                        const std::shared_ptr<AxisIndexedExtent>& cur_ex);

        static inline float CalculateSah(float* left_min, float* left_max,
                                         float* right_min, float* right_max,
                                         const int& e, const int& n_extents);


    };

    inline void BvhHelper::SortExtentsByEveryAxis(std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMin,
                                                       std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMax) {
        int n_extents = ExtentSortMax[0].size();
        for (int i = 0; i < 3; ++i) {
            std::sort(ExtentSortMin[i].begin(), ExtentSortMin[i].end(),
                      [i] (std::shared_ptr<AxisIndexedExtent> e1, std::shared_ptr<AxisIndexedExtent> e2) {
                return e2->Min[i] > e1->Min[i];
            });
            //Assign each extent it's position in every sorted array
            for (int j = 0; j < n_extents; ++j) {
                ExtentSortMin[i][j]->posMin[i] = j;
            }
        }

        for (int i = 0; i < 3; ++i) {
            std::sort(ExtentSortMax[i].begin(), ExtentSortMax[i].end(),
                      [i] (std::shared_ptr<AxisIndexedExtent> e1, std::shared_ptr<AxisIndexedExtent> e2) {
                return e2->Max[i] > e1->Max[i];
            });

            for (int j = 0; j < n_extents; ++j) {
                ExtentSortMax[i][j]->posMax[i] = j;
            }
        }
    }


    inline void BvhHelper::ShrinkVolume(const int& i, const int& e, const int& n_extents,
                                             const std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMin,
                                             const std::vector<std::shared_ptr<AxisIndexedExtent>>* ExtentSortMax,
                                             int* min_idx, int* max_idx,
                                             float* right_min, float* right_max,
                                             const std::shared_ptr<AxisIndexedExtent>& cur_ex) {
        // As we move one extent from right to left, 
        // the right extent also has to recalculate its borders
        // We do it by figuring out if the current extent has more 
        // shrinked length by each axis than current min extent.
        // If so we need to adjust right volume and set the new 
        // index for min and max extents
        AxisIndexedExtent* cur = cur_ex.get();
        AxisIndexedExtent* min_ex = ExtentSortMin[i][min_idx[i]].get();
        if (cur == min_ex) {
            for (int idx = min_idx[i]; idx < n_extents; ++idx) {
                min_ex = ExtentSortMin[i][idx].get();
                int posMax = min_ex->posMax[i];
                if (posMax > e) {
                    min_idx[i] = idx;
                    right_min[i] = ExtentSortMin[i][idx]->Min[i];
                    break;
                }
            }
        }

        for (int a = 1; a < 3; ++a) {
            int n = (a + i) % 3;
            if (cur_ex->Min[n] == ExtentSortMin[n][min_idx[n]]->Min[n]) {
                for (int idx = min_idx[n]; idx < n_extents; ++idx) {
                    if (ExtentSortMin[n][idx]->posMax[i] > e) {
                        min_idx[n] = idx;
                        right_min[n] = ExtentSortMin[n][idx]->Min[n];
                        break;
                    }
                }
            }
        }

        for (int a = 1; a < 3; ++a) {
            int n = (a + i) % 3;
            if (cur_ex->Max[n] == ExtentSortMax[n][max_idx[n]]->Max[n]) {
                for (int idx = max_idx[n]; idx >= 0; --idx) {
                    if (ExtentSortMax[n][idx]->posMax[i] > e) {
                        max_idx[n] = idx;
                        right_max[n] = ExtentSortMax[n][idx]->Max[n];
                        break;
                    }
                }
            }
        }

    }


    inline float BvhHelper::CalculateSah(float* left_min, float* left_max,
                                              float* right_min, float* right_max,
                                              const int& e, const int& n_extents) {
        float left_side[3];
        for (int i = 0; i < 3; ++i) {
            left_side[i] = left_max[i] - left_min[i];
        }

        float left_sah = (e + 1) * (left_side[0] * left_side[1] +
                                    left_side[1] * left_side[2] +
                                    left_side[2] * left_side[0]);

        float right_side[3];
        for (int i = 0; i < 3; ++i) {
            right_side[i] = right_max[i] - right_min[i];
        }

        float right_sah = (n_extents - e - 1) * (right_side[0] * right_side[1] +
                                                 right_side[1] * right_side[2] +
                                                 right_side[2] * right_side[0]);

        return left_sah + right_sah;
    }



    void CalculateBvh(const std::vector<std::shared_ptr<AxisIndexedExtent>>& extents,
                                Bvh* bvh) {
        int n_extents = extents.size();

        if (n_extents <= 2) {
            for (int i = 0; i < n_extents; ++i) {
                bvh->Extents.push_back(static_cast<PolygonExtent>(*(extents[i])));
            }
            return;
        }

        std::vector<std::shared_ptr<AxisIndexedExtent>>
            ExtentSortMin[3] = { extents, extents, extents };
        std::vector<std::shared_ptr<AxisIndexedExtent>>
            ExtentSortMax[3] = { extents, extents, extents };

        BvhHelper::SortExtentsByEveryAxis(ExtentSortMin, ExtentSortMax);


        float min_Sah = FLT_MAX;
        float best_left_min[3];
        float best_left_max[3];
        float best_right_min[3];
        float best_right_max[3];

        int n = 0;
        int axis = 0;

        for (int i = 0; i < 3; ++i) {  // i represents axis (0~x, 1~y, 2~z)

            float left_min[3] = { ExtentSortMax[i][0]->Min.X,
                ExtentSortMax[i][0]->Min.Y,
                ExtentSortMax[i][0]->Min.Z };

            float left_max[3] = { ExtentSortMax[i][0]->Max.X,
                ExtentSortMax[i][0]->Max.Y,
                ExtentSortMax[i][0]->Max.Z };

            // The volume containing all extents  
            float right_min[3] = { ExtentSortMin[0][0]->Min.X,
                ExtentSortMin[1][0]->Min.Y,
                ExtentSortMin[2][0]->Min.Z };
            float right_max[3] = { ExtentSortMax[0][n_extents - 1]->Max.X,
                ExtentSortMax[1][n_extents - 1]->Max.Y,
                ExtentSortMax[2][n_extents - 1]->Max.Z };

            int min_idx[3] = { 0, 0, 0 };
            int max_idx[3] = { n_extents - 1, n_extents - 1, n_extents - 1 };


            for (int e = 0; e < n_extents; ++e) {
                auto cur_ex = ExtentSortMax[i][e];

                // Get new division


                // Left Volume must include all of the previously processed extents,
                // so we need to adjust its borders.
                for (int a = 0; a < 3; ++a) {
                    if (cur_ex->Min[a] < left_min[a]) {
                        left_min[a] = cur_ex->Min[a];
                    }
                    if (cur_ex->Max[a] > left_max[a]) {
                        left_max[a] = cur_ex->Max[a];
                    }
                }

                BvhHelper::ShrinkVolume(i, e, n_extents,
                                             ExtentSortMin, ExtentSortMax,
                                             min_idx, max_idx,
                                             right_min, right_max,
                                             cur_ex);

                float sah = BvhHelper::CalculateSah(left_min, left_max,
                                                         right_min, right_max,
                                                         e, n_extents);

                if (sah < min_Sah) {
                    axis = i;
                    n = e;
                    min_Sah = sah;
                    memcpy(best_left_min, left_min, 3 * sizeof(float));
                    memcpy(best_left_max, left_max, 3 * sizeof(float));
                    memcpy(best_right_min, right_min, 3 * sizeof(float));
                    memcpy(best_right_max, right_max, 3 * sizeof(float));
                }
            }
        }
        std::vector<std::shared_ptr<AxisIndexedExtent>> left_ex;
        std::vector<std::shared_ptr<AxisIndexedExtent>> right_ex;

        left_ex.insert(left_ex.begin(), ExtentSortMax[axis].begin(), ExtentSortMax[axis].begin() + n + 1);
        right_ex.insert(right_ex.begin(), ExtentSortMax[axis].begin() + n + 1, ExtentSortMax[axis].end());


        bvh->LeftNode = std::make_shared<Bvh>();
        bvh->LeftNode->Boundary = Geometry::Extent({ best_left_min[0], best_left_min[1], best_left_min[2] },
        { best_left_max[0], best_left_max[1], best_left_max[2] });
        bvh->RightNode = std::make_shared<Bvh>();
        bvh->RightNode->Boundary = Geometry::Extent({ best_right_min[0], best_right_min[1], best_right_min[2] },
        { best_right_max[0], best_right_max[1], best_right_max[2] });
        CalculateBvh(left_ex, bvh->LeftNode.get());
        CalculateBvh(right_ex, bvh->RightNode.get());
    }
}

// !Bvh.h