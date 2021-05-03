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