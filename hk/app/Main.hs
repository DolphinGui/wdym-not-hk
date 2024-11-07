{-# LANGUAGE TupleSections #-}
module Main where

import Data.Char
import  Tokenizer
import Debug.Trace (trace)

data Operator = Add
    | Sub
    | Mult
    | Div
    | Assign
    | Exp
    deriving (Read, Show, Eq, Ord)

newtype Varname = Varname [Char]
    deriving (Read, Show, Eq, Ord)

data Expression = Number Double
    | Variable Varname
    | Operator Operator
    | Subexpression [Expression]
    deriving (Read, Show, Eq, Ord)

type Context = [(Varname, Double)]

parseOp :: String -> Maybe Operator

parseOp tok
    | tok == "+" = Just Add
    | tok == "-" = Just Sub
    | tok == "*" = Just Mult
    | tok == "/" = Just Div
    | tok == "=" = Just Assign
    | tok == "^" = Just Exp
    | otherwise = Nothing

parseNum :: String -> Maybe Double

parseNum tok
    | all isNumber tok = Just (read tok)
    | otherwise = Nothing

(>>?) :: (t1 -> Maybe t2) -> (t2 -> a) -> t1 -> Maybe a
(>>?) b c a
  = case b a of
      Just r -> Just $ c r
      Nothing -> Nothing

(>?) :: Maybe t2 -> (t2 -> a) -> Maybe a
(>?) m f = case m of
    Just r -> Just $ f r
    Nothing -> Nothing


(?<) ::  (t2 -> a) -> Maybe t2 -> Maybe a
(?<) f m = m >? f

(?:) :: Maybe a -> Maybe [a] -> Maybe [a]
(?:) a b = case (a, b) of
    (Nothing, _) -> Nothing
    (_, Nothing) -> Nothing
    (Just x, Just y) -> Just (x : y)

(?++) :: Maybe [a] -> Maybe [a] -> Maybe [a]
(?++) a b = case (a, b) of
    (Nothing, _) -> Nothing
    (_, Nothing) -> Nothing
    (Just x, Just y) -> Just (x ++ y)

(||?) :: (a -> Maybe b) -> (a -> Maybe b) -> (a -> Maybe b)
(||?) b c a =  case b a of
    Just x -> Just x
    Nothing -> c a

parseExpr :: String -> Maybe Expression
parseExpr  = (parseNum >>? Number) ||? (parseOp >>? Operator) ||? (Just . Variable . Varname)


addImplicitMult :: [Expression] -> [Expression]
addImplicitMult [] = []
addImplicitMult [a] = [a]
addImplicitMult (lhs :(rhs : es)) = if isValue lhs && isValue rhs
    then [lhs, Operator Mult, rhs]
    else [lhs, rhs]
    ++ addImplicitMult es

parse :: [String] -> Maybe [Expression]
parse = parse' >>? addImplicitMult
    where
        parse' :: [String] -> Maybe [Expression]
        parse' [] = Just []
        parse' [tok] =  case parseExpr tok of
            Just expr -> Just [expr]
            Nothing -> Nothing
        parse' toks = if head toks /= "("
            then parseExpr (head toks) ?:  parse' (tail toks)
            else let (inner, outer) = (delimitLastWhen (==")") $ tail toks) in (parse' >>? Subexpression) inner ?: parse' outer

-- empty subexpressions either mean empty parenthensis
-- or unclosed parenthensis
validateExpr :: [Expression] -> Bool
validateExpr = elem (Subexpression [])

isValue ::  Expression -> Bool
isValue e = case e of
    Operator _ -> False
    _ -> True

isSubexpr :: Expression -> Bool
isSubexpr e = case e of
    Subexpression _ -> True
    _ -> False

eval :: (Expression, Context) -> Maybe (Double, Context)
eval (expr, ct) =  case expr of
    Number n -> Just (n, ct)
    Variable v -> case lookup v ct of
        Just n -> Just (n, ct)
        Nothing -> Nothing
    Subexpression sub -> reduce (sub, ct)
    _ -> Nothing

assign :: Varname -> Double -> Context -> Context
assign v d ct =(v, d) : case lookup v ct of
    Nothing ->  ct
    Just old -> filter (== (v, old)) ct

getOp :: Operator -> Context -> Expression -> Expression -> Maybe (Expression, Context)
getOp op ct lhs rhs = case op of
    Assign -> case lhs of
        Variable v -> do
            (r, ct1) <- eval (rhs, ct)
            Just (Number r, assign v r ct1)
        _ -> Nothing
    other -> do
        (l, ct1) <- eval (lhs, ct)
        (r, ct2) <- eval (rhs, ct1)
        Just (Number $ (case other of
            Add -> (+)
            Sub -> (-)
            Mult -> (*)
            Div -> (/)
            Exp -> (**)) l r, ct2)

window :: ((a, a, a) -> Maybe [a]) -> [a] -> Maybe [a]
window f arr
    | (lhs : (mid : ( rhs : rest))) <- arr = case f (lhs, mid, rhs) of
        Just r ->  Just r ?++ window f rest
        Nothing -> Nothing
    | _ <- arr = Just []

concatCt :: Expression -> Maybe ([Expression], Context) -> Maybe ([Expression], Context)
concatCt _ Nothing = Nothing
concatCt left (Just (right, ct)) = Just (left : right, ct)

reduce' :: Operator ->  ([Expression], Context) -> Maybe ([Expression], Context)
reduce' _ ([], ct) = Just ([], ct)
reduce' _ ([a], ct) = Just ([a], ct)
reduce' _ ([a, b], ct) = Just ([a,b], ct)
reduce' o (lhs: (mid : (rhs : es)), ct)
    | Operator op <- mid = if op == o
        then case getOp o ct lhs rhs of
            Nothing -> Nothing
            Just (e, ctnew) -> reduce' o (e : es, ctnew)
        else concatCt lhs $ reduce' o (mid : rhs : es, ct)
    | otherwise = concatCt lhs $ reduce' o (mid : rhs : es, ct)

reduce :: ([Expression], Context) -> Maybe (Double, Context)
reduce iii = case reduce' Exp iii >>= reduce' Mult >>= reduce' Div >>= reduce' Add >>= reduce' Sub >>= reduce' Assign
    of
    Just (expFinal, ctFinal) -> case expFinal of
        [fin] -> eval (fin, ctFinal)
        _ -> Nothing
    Nothing -> Nothing

main :: IO ()
main = do
    putStrLn "Hello, Haskell!"
    let a = parse $ tokenize "x =  (y = 3) * 43 ^ 2 + 1 - 3"
    case a of
        Just e -> print $ reduce (e, [])
        Nothing -> print "Nothing"

    return ()


