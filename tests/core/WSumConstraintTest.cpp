//
// Created by Ferdinando Fioretto on 12/3/16.
//

#include <gtest/gtest.h>

class WSumConstraintTest : public ::testing::Test
{
public:
    void SetUp(const std::string& strDoc)
    {

    }

protected:
};


TEST_F(WSumConstraintTest, checcomponent)
{
const std::string xmlString =
        "<iidm:network xmlns:iidm=\"http://www.itesla_project.eu/schema/iidm/1_0\">\n"
                "<iidm:busbarSection id=\"V\" name=\"W\" node=\"0\" v=\"64.37501\" angle=\"-3.4991434\"/>\n"
                "</iidm:network>";

SetUp(xmlString);

//auto jNet = grg_busbar->getJNetComponent();
//EXPECT_STREQ(jNet["source_id"].asCString(), iidm_busbar->getId().c_str());
//EXPECT_STREQ(jNet["name"].asCString(), iidm_busbar->getName().c_str());
//EXPECT_STREQ(jNet["type"].asCString(), "busbar");
//EXPECT_STREQ(jNet["link_node"].asCString(), iidm_busbar->getNode().c_str());
//
//EXPECT_EQ(jNet["status"], jsonutils::getOnOffVar());
//EXPECT_EQ(jNet["voltage"]["angle"], jsonutils::getVarBounds(-30, 30));
//EXPECT_EQ(jNet["voltage"]["magnitude"], jsonutils::getVarBounds());
}
