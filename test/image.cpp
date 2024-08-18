#include <filesystem>

#include <catch2/catch_test_macros.hpp>
#include <trm/image.hpp>

TEST_CASE("Image Reading", "[image][io]") {
  SECTION("Valid Image File") {
    auto image = trm::Image::read(ASSET_DIR "valid_image_path.hdr");
    REQUIRE(image.has_value());
    CHECK(image.value().md5() == "69eb943fa4b1467ac6c1b079c4028567");
  }

  SECTION("Non-existent Image File") {
    auto image = trm::Image::read("non_existent_image_path.hdr");
    REQUIRE_FALSE(image.has_value());
  }

  SECTION("Unsupported Image Format") {
    auto image = trm::Image::read("invalid_image_path.unsupported");
    REQUIRE_FALSE(image.has_value());
  }
}

TEST_CASE("Image Writing", "[image][io]") {
  trm::Image image(100, 100, 3); // Create a dummy image

  SECTION("Valid Path") { REQUIRE(image.write("output_image_path.hdr")); }

  SECTION("Invalid Path") { REQUIRE_FALSE(image.write("/invalid_path/output_image_path.hdr")); }

  SECTION("Unsupported Format") { REQUIRE_FALSE(image.write("output_image_path.unsupported")); }
}

TEST_CASE("Image Normalization", "[image]") {
  trm::Image image(100, 100, 3); // Create a dummy image

  SECTION("Color Buffer") {
    image.buffer = trm::Buffer::COLOR;
    auto normalized_data = image.normalized();
    REQUIRE(normalized_data.size() == image.data.size());
  }

  SECTION("Albedo Buffer") {
    image.buffer = trm::Buffer::ALBEDO;
    auto normalized_data = image.normalized();
    REQUIRE(normalized_data.size() == image.data.size());
  }

  SECTION("Normal Buffer") {
    image.buffer = trm::Buffer::NORMAL;
    auto normalized_data = image.normalized();
    REQUIRE(normalized_data.size() == image.data.size());
  }

  SECTION("Depth Buffer") {
    image.buffer = trm::Buffer::DEPTH;
    auto normalized_data = image.normalized();
    REQUIRE(normalized_data.size() == image.data.size());
  }
}
