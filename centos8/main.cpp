#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/BatchGetItemRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/DeleteItemRequest.h>
#include <aws/dynamodb/model/ScanRequest.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <aws/dynamodb/model/DeleteTableRequest.h>

#include <gtest/gtest.h>

using namespace Aws::DynamoDB::Model;
using namespace Aws::Utils::Json;

class FooTest : public ::testing::Test {
protected:
  std::unique_ptr<Aws::DynamoDB::DynamoDBClient> dynamodb;
  Aws::SDKOptions options;
  FooTest() {}
  virtual ~FooTest() {}
  virtual void SetUp() {
    Aws::InitAPI(options);
    setenv("AWS_EC2_METADATA_DISABLED", "true", 1);
    Aws::Client::ClientConfiguration dynamodb_clientConfig;
    dynamodb_clientConfig.endpointOverride = "127.0.0.1:7000";
    dynamodb_clientConfig.scheme = Aws::Http::Scheme::HTTP;
    dynamodb_clientConfig.region = Aws::String("us-west-1");
    dynamodb = std::make_unique<Aws::DynamoDB::DynamoDBClient>(Aws::Auth::AWSCredentials("fakeMyKeyId", "fakeSecretAccessKey"), dynamodb_clientConfig);
  }
  virtual void TearDown() {
    Aws::ShutdownAPI(options);
  }
};

TEST_F(FooTest, update) {
  if(dynamodb) {
    Aws::DynamoDB::Model::UpdateItemRequest req;

    Aws::Vector<std::shared_ptr<AttributeValue>> list;
    auto item = std::make_shared<AttributeValue>();
    auto key = std::make_shared<AttributeValue>();
    key->SetS("may_val3");
    item->AddMEntry("my_key3", key);
    list.push_back(item);

    Aws::Vector<std::shared_ptr<AttributeValue>> init_list;
    auto init_item = std::make_shared<AttributeValue>();
    init_list.push_back(init_item);

    req.WithTableName("table1").WithKey({{"oid",AttributeValue().SetS("444")}})
      .WithExpressionAttributeNames({{"#a", "Date"}})
      .WithExpressionAttributeValues({{":val", AttributeValue().SetL(list)},
                                      {":init", AttributeValue().SetL(init_list)}})
      .WithUpdateExpression("SET #a = list_append(if_not_exists(#a, :init), :val)");
    auto result = dynamodb->UpdateItem(req);
    if (result.IsSuccess())
    {
      std::cout << "update success" << std::endl;
    }
    else
    {
      std::cout << "update failed" << std::endl;
    }
  }
}

TEST_F(FooTest, update2) {
  if(dynamodb) {
    Aws::DynamoDB::Model::UpdateItemRequest req;

    auto item = std::make_shared<AttributeValue>();
    auto key = std::make_shared<AttributeValue>();
    key->SetS("may_val3");
    item->AddMEntry("my_key3", key);

    req.WithTableName("table1").WithKey({{"oid",AttributeValue().SetS("666")}})
      .WithExpressionAttributeNames({{"#a", "Omap"}})
      .WithExpressionAttributeValues({{":val", AttributeValue().SetM(item->GetM())},
                                      {":init", AttributeValue().SetM({})}})
      .WithUpdateExpression("SET #a = if_not_exists(#a, :init) + :val");
    auto result = dynamodb->UpdateItem(req);
    if (result.IsSuccess())
    {
      std::cout << "update success" << std::endl;
    }
    else
    {
      std::cout << "update failed" << std::endl;
    }
  }
}

TEST_F(FooTest, update_add) {
  if(dynamodb) {
    Aws::DynamoDB::Model::UpdateItemRequest req;

    auto item = std::make_shared<AttributeValue>();
    auto key = std::make_shared<AttributeValue>();
    key->SetS("may_val3");
    item->AddMEntry("my_key3", key);

    req.WithTableName("table1").WithKey({{"oid",AttributeValue().SetS("777")}})
      .WithExpressionAttributeNames({{"#a", "Omap"}})
      .WithExpressionAttributeValues({
        {":val", AttributeValue().SetSS({"222222"})},
//        {":init", AttributeValue().SetSS({"header"})}
      })
      .WithUpdateExpression("ADD #a :val");
    auto result = dynamodb->UpdateItem(req);
    ASSERT_TRUE(result.IsSuccess());
//    if (result.IsSuccess())
//    {
//      std::cout << "update success" << std::endl;
//    }
//    else
//    {
//      std::cout << "update failed" << std::endl;
//    }
  }
}

TEST_F(FooTest, update_rm) {
  if(dynamodb) {
    Aws::DynamoDB::Model::UpdateItemRequest req;

    Aws::Vector<std::shared_ptr<AttributeValue>> list;
    auto item = std::make_shared<AttributeValue>();
    auto key = std::make_shared<AttributeValue>();
    key->SetS("may_val2");
    item->AddMEntry("my_key2", key);
    list.push_back(item);

    Aws::Vector<std::shared_ptr<AttributeValue>> init_list;
    auto init_item = std::make_shared<AttributeValue>();
    init_list.push_back(init_item);

    req.WithTableName("table1").WithKey({{"oid",AttributeValue().SetS("444")}})
      .WithExpressionAttributeNames({{"#a", "Date"}})
//      .WithExpressionAttributeValues({{":val", AttributeValue().SetL(list)}})
      .WithUpdateExpression("REMOVE #a[0]");
    auto result = dynamodb->UpdateItem(req);
    if (result.IsSuccess())
    {
      std::cout << "update success" << std::endl;
    }
    else
    {
      std::cout << "update failed" << std::endl;
    }
  }
}

TEST_F(FooTest, createt) {
  if(dynamodb) {
    Aws::DynamoDB::Model::CreateTableRequest req;
    Aws::DynamoDB::Model::AttributeDefinition haskKey;
    haskKey.SetAttributeName("Name");
    haskKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);
    req.AddAttributeDefinitions(haskKey);

    Aws::DynamoDB::Model::KeySchemaElement keyscelt;
    keyscelt.WithAttributeName("Name").WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
    req.AddKeySchema(keyscelt);

    Aws::DynamoDB::Model::ProvisionedThroughput thruput;
    thruput.WithReadCapacityUnits(5).WithWriteCapacityUnits(5);
    req.SetProvisionedThroughput(thruput);
    req.SetTableName("table1");
    const Aws::DynamoDB::Model::CreateTableOutcome& result = dynamodb->CreateTable(req);
    if (result.IsSuccess())
    {
      std::cout << "Table \"" << result.GetResult().GetTableDescription().GetTableName() <<
                " created!" << std::endl;
    }
    else
    {
      std::cout << "Failed to create table: " << result.GetError().GetMessage();
    }
  }
}

TEST_F(FooTest, deletet) {
  if(dynamodb) {
    Aws::DynamoDB::Model::DeleteTableRequest req;
    req.WithTableName("table1");
    const Aws::DynamoDB::Model::DeleteTableOutcome & result = dynamodb->DeleteTable(req);
    if (result.IsSuccess())
    {
      std::cout << "Table \"" << result.GetResult().GetTableDescription().GetTableName() <<
                " deleted!" << std::endl;
    }
    else
    {
      std::cout << "Failed to delete table: " << result.GetError().GetMessage();
    }
  }
}

int main(int argc, char **argv) {
  Aws::Utils::Logging::InitializeAWSLogging(Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>("RunUnitTests", Aws::Utils::Logging::LogLevel::Debug));
  testing::InitGoogleTest(&argc, argv);
  int exitCode = RUN_ALL_TESTS();
  Aws::Utils::Logging::ShutdownAWSLogging();
  return exitCode;
}