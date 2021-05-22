#include <aws/core/Aws.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/Outcome.h>

#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/BatchGetItemRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/DeleteItemRequest.h>
#include <aws/dynamodb/model/ScanRequest.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <gtest/gtest.h>

using namespace Aws::DynamoDB::Model;
using namespace Aws::Utils::Json;

class FooTest : public ::testing::Test {

protected:
  Aws::DynamoDB::DynamoDBClient * dynamodbclient;
  Aws::SDKOptions options;

  FooTest() {}
  virtual ~FooTest() {}

  virtual void SetUp() {
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration dynamodb_clientConfig;
    dynamodb_clientConfig.endpointOverride = "127.0.0.1:7000";
    dynamodb_clientConfig.scheme = Aws::Http::Scheme::HTTP;
//    dynamodb_clientConfig.region = Aws::String("us-east-1");
    dynamodbclient = new Aws::DynamoDB::DynamoDBClient(Aws::Auth::AWSCredentials("fakeMyKeyId", "fakeSecretAccessKey"), dynamodb_clientConfig);
  }
  virtual void TearDown() {
    Aws::ShutdownAPI(options);
    delete dynamodbclient;
  }
};

TEST_F(FooTest, createt) {
  if(dynamodbclient) {
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
    const Aws::DynamoDB::Model::CreateTableOutcome& result = dynamodbclient->CreateTable(req);
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

int main(int argc, char **argv) {
  Aws::Utils::Logging::InitializeAWSLogging(
    Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
      "RunUnitTests", Aws::Utils::Logging::LogLevel::Debug));
  testing::InitGoogleTest(&argc, argv);
  int exitCode = RUN_ALL_TESTS();
  Aws::Utils::Logging::ShutdownAWSLogging();
  return exitCode;
}