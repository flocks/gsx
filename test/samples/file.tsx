const Test = () => {
  return (
    <Box variant="primary" onClick={() => console.log("test")}>
      <div>
        <Button variant="primary" size="large">
          Button
        </Button>
        <Button size="large">Button</Button>
        <Button variant="secondary">Button</Button>
      </div>
    </Box>
  );
};
