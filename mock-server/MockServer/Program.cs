var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.MapGet("/speed", () => new SpeedMessage(
    Sog: 15.0 * new Random().NextDouble(),
    Cog: 359.0 * new Random().NextDouble()
))
.WithName("GetSpeed");

app.Run();

record SpeedMessage(double Sog, double Cog);
