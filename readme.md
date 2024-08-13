# Direct Debit - Subscription Hook

### Disclaimer

This is the first iteration of Direct debit. It needs a lot of testing and re-iteration.

**Live on Xahau Testnet**

[xrplwin testnet](https://xahau-testnet.xrplwin.com/account/rJbSHGgJmEvHcQiRrzyhdJneRr6Vh3h5v6)

[xahau-test](https://explorer.xahau-test.net/rJbSHGgJmEvHcQiRrzyhdJneRr6Vh3h5v6/tx)

### You can currencly perform 3 operations

**Own Action**

1. Subscribe to an account/business/charity.
2. Unsubscribe from an account/business/charity.

**Anybody**

3. Direct debt from your account to the subscribed account/business/charity.

### Installing the Hook on your account

```
import { Client, Wallet, SetHookFlags } from "@transia/xrpl";
import {
  createHookPayload,
  setHooksV3,
  SetHookParams,
} from "@transia/hooks-toolkit";

export async function main(): Promise<void> {
  const serverUrl = "wss://xahau-test.net";   // Xahau Test Network
  const client = new Client(serverUrl);
  await client.connect();
  client.networkID = await client.getNetworkID();
  const secret = "" // Your account secret
  const hookWallet = Wallet.fromSeed(secret);

  const hook1 = createHookPayload({
    version: 0,
    createFile: "subscription",
    namespace: "DD",
    flags: SetHookFlags.hsfOverride,
    hookOnArray: ["Invoke"],
  });

  await setHooksV3({
    client: client,
    seed: hookWallet.seed,
    hooks: [{ Hook: hook1 }],
  } as SetHookParams);

  await client.disconnect();
}

main();
```

### Hook Install, using HookHash

(The hookhash is on Xahau Test network - links above)

```
{
    Account: your.raddress,
    TransactionType: "SetHook",
    Fee: "2000000",
    Hooks:
    [
        {
            Hook: {
                HookHash: "A83FC2584598E08075B3476A323E54451D6EDD2157B5BFFE1F13C4C69486441A",
                HookOn: "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFFFFBFFFFF",
                HookNamespace: "92F089F2A70DF5D960AAC7C83DAC8BC454C2CB1FA9A8E3061A8F6A84F338F2D1"
            }
        }
    ],
    ...networkInfo.txValues
}
```

### Transaction Parameter

**B** or hex **42**: Business account ID - to subscribe. (20 byte account ID)

**NUM** or hex **4E554D**: Subscription number. Fetch this from the users state. (1 byte HEX as value)

**AMT** or hex **414D54**: represents Amount field in the Payment payload. (< xlf 8b req amount, 20b currency, 20b issuer > or native currency)

### AMT

1. Issued Currency:
   00008D49FD1A87540000000000000000000000005553440000000000A407AF5856CCF3C42619DAA925813FC955C72983
   (28 bytes in total)

   00008D49FD1A8754: 2 (8 bytes)

   A407AF5856CCF3C42619DAA925813FC955C72983: rExKpRKXNz25UAjbckCRtQsJFcSfjL9Er3 (20 bytes)

   0000000000000000000000005553440000000000: USD (20 bytes)

2. Native Currency:

   Just pass in the amount (8 bytes) - example: 00008D49FD1A8754 (equivalent to 2) [Float to XFL](https://hooks.services/tools/float-to-xfl)

## Sample Code Snippet

**Subscribe** (Self-invoke)

```
{
  "Fee": "1137",
  "HookParameters": [
    {
      "HookParameter": {
        "HookParameterName": "414D54", // AMT
        "HookParameterValue": "00008D49FD1A87540000000000000000000000005553440000000000A407AF5856CCF3C42619DAA925813FC955C72983"
      }
    },
    {
      "HookParameter": {
        "HookParameterName": "42", // B
        "HookParameterValue": "B675B3DE010A7C7A2DF655C54C284A2113FFD76B"
      }
    }
  ],
  "TransactionType": "Invoke",
  "Account": your.raddress,
   ...networkInfo.txValues
}
```

**Unsubscribe** (Self-invoke)

```
{
  "Fee": "1090",
  "HookParameters": [
    {
      "HookParameter": {
        "HookParameterName": "4E554D", // NUM
        "HookParameterValue": "02"     // HEX Value. 02 is 2. 0A is 10.
      }
    },
    {
      "HookParameter": {
        "HookParameterName": "42", // B
        "HookParameterValue": "B675B3DE010A7C7A2DF655C54C284A2113FFD76B"
      }
    }
  ],
  "TransactionType": "Invoke",
  "Account": your.raddress,
   ...networkInfo.txValues
}
```

**Debit** (Invoke by anyone, on your account)

```
{
  "Destination": your.raddress,
  "Fee": "1090",
  "HookParameters": [
    {
      "HookParameter": {
        "HookParameterName": "4E554D", // NUM
        "HookParameterValue": "14"    //  hex 14 is 20 decimal. It's pointing to the subscription at 20th position in your hook state.
      }
    },
    {
      "HookParameter": {
        "HookParameterName": "42", // B
        "HookParameterValue": "B675B3DE010A7C7A2DF655C54C284A2113FFD76B"
      }
    }
  ],
  "TransactionType": "Invoke",
  "Account": anyone.raddress,
   ...networkInfo.txValues
}
```

### Namespace:

1. 92F089F2A70DF5D960AAC7C83DAC8BC454C2CB1FA9A8E3061A8F6A84F338F2D1

- businesses subscribed to and number of subscriptions with each business.

2. B1CE0E75AE18223D25161E4B646F6FC03D5CFE51AE8E2D68B1AE8765658382C5

- subscription information

3. AA71EAB4F7DDF704AA715D9781F2901DB5A4D43E6F3FB318DFAA93CE11946249

- paid or not.

### Features:

1. You can subscribe to as many businesses as you wish.
2. You can subscribe to same business with multiple assets. Ex: You can subscribe to USD as well as EUR ..and more
3. You can unsubscribe anytime.
4. Anyone can read the hook state on your account and invoke the debit.
5. For each subscription, the debit can only occur once per month.
