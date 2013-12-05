#ifndef CURFIL_RANDOM_FOREST_IMAGE_H
#define CURFIL_RANDOM_FOREST_IMAGE_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "random_tree_image.h"

namespace curfil {

class TreeNodes;

/**
 * A random forest for RGB-D images.
 */
class RandomForestImage {

public:

    /**
     * Load the random forest from disk.
     *
     * @param treeFiles paths of JSON tree files
     * @param deviceIds the GPU device IDs to use for prediction
     * @param accelerationMode the acceleration mode to use for prediction. CPU or GPU.
     * @param histogramBias if larger than 0.0, apply a histogram bias (see the Wiki) after loading the forest.
     */
    explicit RandomForestImage(const std::vector<std::string>& treeFiles,
            const std::vector<int>& deviceIds = std::vector<int>(1, 0),
            const AccelerationMode accelerationMode = GPU_ONLY,
            const double histogramBias = 0.0);

    /**
     * Prepare a random forest with the given number of trees and the configuration.
     *
     * @param treeCount the number of trees in the forest
     * @param configuration the configuration to use for training
     */
    explicit RandomForestImage(unsigned int treeCount,
            const TrainingConfiguration& configuration);

    /**
     * Construct a random forest from the given list of existing trees
     *
     * @param ensemble list of trees that should be used to construct the random forest from
     * @param configuration the configuration the trees were trained with
     */
    explicit RandomForestImage(const std::vector<boost::shared_ptr<RandomTreeImage> >& ensemble,
            const TrainingConfiguration& configuration);

    /**
     * Starts the training process of the random forest.
     *
     * @param trainLabelImages the list of labeled images to train the random forest from
     * @param trainTreesSequentially whether to train the trees in the random forest in parallel or sequentially.
     */
    void train(const std::vector<LabeledRGBDImage>& trainLabelImages, bool trainTreesSequentially = false);

    /**
     * @param image the image which should be classified
     * @param if not null, probabilities per class in a C×H×W matrix for C classes and an image of size W×H
     * @return prediction image which has the same size as 'image'
     */
    LabelImage predict(const RGBDImage& image,
            cuv::ndarray<float, cuv::host_memory_space>* prediction = 0,
            const bool onGPU = true, bool useDepthImages = true,  cuv::ndarray<size_t, cuv::host_memory_space>* nodeOffsets = 0) const;

    /**
     * @return a recursive sum of per-feature type count
     */
    std::map<std::string, size_t> countFeatures() const;

    /**
     * @return the number of classes (labels) that random forest was trained from
     */
    LabelType getNumClasses() const;

    /**
     * @return the n-th tree in the forest where 0 ≤ treeNr < numTrees
     */
    const boost::shared_ptr<RandomTreeImage> getTree(size_t treeNr) const {
#ifdef NDEBUG
        return ensemble[treeNr];
#else
        return ensemble.at(treeNr);
#endif
    }

    /**
     * @return the list of trees in the forest
     */
    const std::vector<boost::shared_ptr<RandomTreeImage> >& getTrees() const {
        return ensemble;
    }

    /**
     * @return the configuration the random forest was trained with
     */
    const TrainingConfiguration& getConfiguration() const {
        return configuration;
    }

    bool shouldIgnoreLabel(const LabelType& label) const;

    std::map<LabelType, RGBColor> getLabelColorMap() const;

    void normalizeHistograms(const double histogramBias);

private:

    TrainingConfiguration configuration;

    std::vector<boost::shared_ptr<RandomTreeImage> > ensemble;
    std::vector<boost::shared_ptr<const TreeNodes> > treeData;
    boost::shared_ptr<cuv::allocator> m_predictionAllocator;
};

}

std::ostream& operator<<(std::ostream& os, const curfil::RandomForestImage& ensemble);

#endif
